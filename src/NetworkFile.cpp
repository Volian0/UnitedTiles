#include "NetworkFile.h"

#include "Path.h"

#include <curl/curl.h>
#include <picosha2/picosha2.h>
#include <SDL/SDL.h>

#include <array>
#include <utility>
#include <filesystem>
#include <algorithm>
#include <fstream>
#include <vector>
#include <iostream> //debug

std::mutex NetworkFile::m_global_mutex;

NetworkFile::NetworkFile(std::string t_url, std::string t_output_filename, std::string t_expected_hash)
:url{std::move(t_url)},
output_filename{std::move(t_output_filename)},
expected_hash{std::move(t_expected_hash)},
m_download_thread{download, this}
{
}

NetworkFile::~NetworkFile()
{
    m_global_mutex.unlock();
    //std::cout << "Canceling download..." << std::endl;
    cancel_download();
    //std::cout << "Waiting for cancel..." << std::endl;
    m_download_thread.join();
    m_global_mutex.lock();
    //std::cout << "File destroyed..." << std::endl;
}

NetworkFile::State NetworkFile::get_state() const
{
    std::scoped_lock lock{m_local_mutex};
    return m_state;
}

void NetworkFile::cancel_download()
{
    std::scoped_lock lock{m_local_mutex};
    m_cancel_download = true;
}

Number NetworkFile::get_progress() const
{
    std::scoped_lock lock{m_local_mutex};
    return m_progress;
}

NetworkFile::Info NetworkFile::get_info() const
{
    std::scoped_lock lock{m_local_mutex};
    return Info{m_progress, m_state, m_cancel_download};
}

bool NetworkFile::wait_for_download() const
{
    while (true)
    {
        const State state = get_state();
        if (state != State::DOWNLOADING)
        {
            return state == State::SUCCESS;
        }
        std::this_thread::yield();
    }
}

int NetworkFile::progress_callback(void* t_client_ptr, curl_off_t t_dl_total, curl_off_t t_dl_now, curl_off_t t_ul_total, curl_off_t t_ul_now)
{
    //std::cout << "Progress callback" << std::endl;
    NetworkFile* network_file = reinterpret_cast<NetworkFile*>(t_client_ptr);
    std::scoped_lock lock{network_file->m_local_mutex};
    if (network_file->m_cancel_download)
    {
        //std::cout << "Callback ended" << std::endl;
        //network_file.m_state = State::FAILED;
        return 1;
    }
    //std::cout << t_dl_total << std::endl;
    if (t_dl_total == 0 || t_dl_now > t_dl_total)
    {
        network_file->m_progress = 0.0L;
    }
    else
    {
        network_file->m_progress = std::clamp(Number(t_dl_now) / Number(t_dl_total), 0.0L, 1.0L);
    }
    return 0;
}

//std::mutex std_mutex;

extern std::atomic_bool mutex_network_usage;

std::size_t NetworkFile::write_callback(char* t_ptr, std::size_t t_size, std::size_t t_nmemb, void* t_userdata)
{
    //std::cout << "Write callback" << std::endl;
    mutex_network_usage = true;
    std::scoped_lock lock{m_global_mutex};
    mutex_network_usage = false;
    const auto total_size = t_size * t_nmemb;
    if (reinterpret_cast<std::ofstream*>(t_userdata)->write(t_ptr, t_size * t_nmemb))
    {
        if (reinterpret_cast<std::ofstream*>(t_userdata)->tellp() > 50000000)
        {
            return CURLE_WRITE_ERROR;        
        }
        return t_size * t_nmemb;
    }
    //const std::size_t written = fwrite(t_ptr, t_size, t_nmemb, reinterpret_cast<FILE*>(t_userdata));
    return CURLE_WRITE_ERROR;
}

void NetworkFile::download(NetworkFile* t_network_file)
{
    //std::cout << "Locking global mutex..." << std::endl;
    mutex_network_usage = true;
    m_global_mutex.lock();
    mutex_network_usage = false;
    //std::cout << "Download started" << std::endl;
    std::filesystem::create_directories(Path::user({}, "downloaded"));
    std::filesystem::create_directories(Path::user({}, "downloading"));

    bool success = false;
    //const auto temp_path = std::filesystem::temp_directory_path();
    //const auto output_filename = temp_path / t_network_file->output_filename; //TODO: sha256 output filename
    const auto output_filename = Path::user(t_network_file->output_filename, "downloaded");
    const auto tmp_filename = Path::user(t_network_file->output_filename, "downloading");
    //std::array<char, CURL_ERROR_SIZE> error_buffer{0};
    if (auto curl_handle = curl_easy_init())
    {
        
        if (std::ofstream file{tmp_filename, std::ios::binary})
        {
            curl_easy_setopt(curl_handle, CURLOPT_URL, t_network_file->url.c_str());
            curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, NetworkFile::write_callback);
            curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &file);
            curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 0);
            curl_easy_setopt(curl_handle, CURLOPT_XFERINFOFUNCTION, NetworkFile::progress_callback);
            curl_easy_setopt(curl_handle, CURLOPT_XFERINFODATA, t_network_file);
            curl_easy_setopt(curl_handle, CURLOPT_NOSIGNAL, 1L);
            //curl_easy_setopt(curl_handle, CURLOPT_ERRORBUFFER, error_buffer.data());

#ifdef __ANDROID__
            curl_easy_setopt(curl_handle, CURLOPT_CAINFO, Path::user("cacert.pem", "extracted/misc/").data());
#endif

            m_global_mutex.unlock();
            success = (curl_easy_perform(curl_handle) == CURLE_OK && file);
            //std::cout << "Action ended" << std::endl;
            //mutex_network_usage = true;
            //std::cout << "Trying to lock global mutex..." << mutex_network_usage <<  std::endl;
            //m_global_mutex.lock();
            //std::cout << "Local mutex locked" << std::endl;
            //mutex_network_usage = false;
        }
        curl_easy_cleanup(curl_handle);
    }
    mutex_network_usage = true;
    std::scoped_lock lock{m_global_mutex, t_network_file->m_local_mutex};
    mutex_network_usage = false;
    //std::scoped_lock lock{};
    if (success && !t_network_file->expected_hash.empty()) //check SHA-256
    {
        std::ifstream file(tmp_filename, std::ios::binary);
        std::vector<unsigned char> hash(picosha2::k_digest_size);
        picosha2::hash256(file, hash.begin(), hash.end());
        const std::string hex_str = picosha2::bytes_to_hex_string(hash.begin(), hash.end());
        if (hex_str != t_network_file->expected_hash)
        {
            std::cout << "Hash mismatch: " << hex_str << " != " << t_network_file->expected_hash << std::endl;
            success = false;
        }
    }
    if (success)
    {
        //const auto destination_path = Path::user(t_network_file->output_filename, "downloaded");
        //std::filesystem::create_directories(std::filesystem::path(output_filename).remove_filename());
        //std::filesystem::copy_file(output_filename, destination_path, std::filesystem::copy_options::overwrite_existing);
        //std::filesystem::remove(output_filename);
        std::filesystem::rename(tmp_filename, output_filename);

        t_network_file->m_progress = 1.0L;
        t_network_file->m_state = State::SUCCESS;
    }
    else
    {
        t_network_file->m_state = State::FAILED;
    }
    //std::string_view error_msg{error_buffer.data()};
    //if (!error_msg.empty())
    //{
    //    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "United Tiles - Network Error", error_msg.data(), nullptr);
    //}
    //m_global_mutex.unlock();
}