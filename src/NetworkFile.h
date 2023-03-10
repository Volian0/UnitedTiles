#pragma once

#include "NonCopyable.h"
#include "Path.h"
#include "Types.h"

//#include <curl/curl.h>

#include <mutex>
#include <thread>
#include <optional>
//#include <atomic>
//#include <cstdio>
//#include <filesystem>

#include <filesystem> //not final

class NetworkFile : NonCopyable
{
    friend class Game;

public:
    enum class State : std::uint8_t
    {
        DOWNLOADING, FAILED, SUCCESS
    };

    struct Info
    {
        Number progress;
        State state;
        bool cancel_download;
    };

public:
    NetworkFile(std::string t_url, std::string t_output_filename, std::string t_expected_hash = {});
    ~NetworkFile();

    [[nodiscard]] State get_state() const;
    void cancel_download();
    [[nodiscard]] Number get_progress() const;

    bool wait_for_download() const;

    Info get_info() const;

    const std::string url;
    const std::string output_filename;
    const std::string expected_hash;

private:
    static int progress_callback(void* t_client_ptr, std::int64_t t_dl_total, std::int64_t t_dl_now, std::int64_t t_ul_total, std::int64_t t_ul_now);
    static std::size_t write_callback(char* t_ptr, std::size_t t_size, std::size_t t_nmemb, void* t_userdata);
    static void download(NetworkFile* t_network_file);

    //void* m_curl_easy_handle = nullptr;
    //FILE* m_file_handle = nullptr;

    //mutable std::mutex m_mutex_cancel_download;
    //mutable std::mutex m_mutex_progress;
    //mutable std::mutex m_mutex_downloaded;

    static std::mutex m_global_mutex;
    mutable std::mutex m_local_mutex;

    Number m_progress = 0.0L;
    State m_state = State::DOWNLOADING;
    bool m_cancel_download = false;

    std::thread m_download_thread;
};

class NetworkFileWrapper : NonCopyable
{
public:
    enum class State : std::uint8_t
    {
        NO_FILE, EXISTS, DOWNLOADING, FAILED
    };

    struct Info
    {
        State state;
        Number progress;

        [[nodiscard]] constexpr auto can_cancel() const -> bool
        {
            return (state == State::DOWNLOADING);
        }

        [[nodiscard]] constexpr auto exists() const -> bool
        {
            return (state == State::EXISTS);
        }

        [[nodiscard]] constexpr auto can_download() const -> bool
        {
            return (state == State::NO_FILE || state == State::FAILED);
        }

        [[nodiscard]] constexpr auto can_remove() const -> bool
        {
            return exists();
        }

        [[nodiscard]] constexpr auto failed() const -> bool
        {
            return state == State::FAILED;
        }
    };

public:
    inline NetworkFileWrapper(std::string t_url, std::string t_output_filename, std::string t_expected_hash = {})
    :url{std::move(t_url)},
    output_filename{std::move(t_output_filename)},
    expected_hash{std::move(t_expected_hash)}
    {
        m_exists = std::filesystem::exists(Path::user(output_filename, "downloaded"));
    }

    ~NetworkFileWrapper() = default;

    //[[nodiscard]] bool exists() const;

    inline Info get_info() const
    {
        if (m_network_file)
        {
            const auto info = m_network_file->get_info();
            if (info.state == NetworkFile::State::FAILED)
            {
                return Info{State::FAILED, info.progress};
            }
            else if (info.state == NetworkFile::State::DOWNLOADING)
            {
                return Info{State::DOWNLOADING, info.progress};
            }
            else if (info.state == NetworkFile::State::SUCCESS)
            {
                return Info{State::EXISTS, 1.0L};
            }
        }
        else
        {
            if (m_exists)
            {
                return Info{State::EXISTS, 1.0L};
            }
            else
            {
                return Info{State::NO_FILE, 0.0L};
            }
        }
    }

    inline void remove()
    {
        m_network_file.reset();
        std::filesystem::remove(Path::user(output_filename, "downloaded"));
        m_exists = false;
    }

    inline void cancel()
    {
        if (!m_network_file)
            return;
        m_network_file.reset();
        m_exists = std::filesystem::exists(Path::user(output_filename, "downloaded"));
    }

    inline void download()
    {
        m_network_file.emplace(url, output_filename, expected_hash);
    }

    const std::string url;
    const std::string output_filename;
    const std::string expected_hash;

private:
    bool m_exists;
    std::optional<NetworkFile> m_network_file;
};