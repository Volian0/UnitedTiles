#include "SongInfo.cpp"
#include "File.cpp"
#include "Path.cpp"

#define CSV_IO_NO_THREAD
#include "csv.h"

#include <stdexcept>
#include <iostream>
#include <string_view>
#include <map>
#include <string>
#include <set>
#include <filesystem>

auto split_string(std::string_view t_string, std::string_view t_delimiter, bool t_remove_empty) -> std::vector<std::string>
{
	if (t_delimiter.empty())
	{
		throw std::runtime_error("Delimiter is empty");
	}
	std::vector<std::string> strings;
	while (true)
	{
		const auto position = t_string.find(t_delimiter);
		if (position == std::string_view::npos)
		{
			break;
		}
		const auto token = t_string.substr(0, position);
		if (!t_remove_empty || !token.empty())
		{
			strings.emplace_back(token);
		}
		t_string.remove_prefix(position + t_delimiter.size());
	}
	if (!t_remove_empty || !t_string.empty())
	{
		strings.emplace_back(t_string);
	}
	return strings;
}

int main(int args_size, char** args)
try
{
    std::map<std::uint16_t, ComposerInfo> composers;
    std::map<std::string, std::uint16_t> composers_name_map;

    std::map<std::uint16_t, SongBasicInfo> songs;
    std::map<std::string, std::uint16_t> songs_name_map;


    std::cout << "doing composers" << std::endl;
    {
        io::CSVReader<5, io::trim_chars<' ', '\t'>, io::double_quote_escape<',', '"'>> csv("United Tiles Songs - Composers.csv");
            csv.read_header(io::ignore_extra_column, "ID", "Name", "Description", "Birth year", "Death year");
        std::string id, name, description, birth, death;

        while (csv.read_row(id, name, description, birth, death))
        {
            if (birth.empty())
            {
                birth = "0";
            }
            if (death.empty())
            {
                death = "0";
            }
            std::uint16_t id_ = std::stoull(id);
            if (composers.contains(id_))
            {
                throw std::runtime_error("Duplicated composer id " + id);
            }
            if (composers_name_map.contains(name))
            {
                throw std::runtime_error("Duplicated composer name " + name);
            }
            composers.emplace(id_, ComposerInfo{name, description, std::stoull(birth), std::stoull(death)});
            composers_name_map.emplace(name, id_);
        }
    }
    std::cout << "doing songs" << std::endl;

    {
        io::CSVReader<7, io::trim_chars<' ', '\t'>, io::double_quote_escape<',', '"'>> csv("United Tiles Songs - Songs.csv");
            csv.read_header(io::ignore_extra_column, "ID", "Name", "Composers", "Description", "Creation Year", "Maker", "Status");
        std::string id, name, composers, description, creation_year, maker, status;

        while (csv.read_row(id, name, composers, description, creation_year, maker, status))
        {
            if (creation_year.empty())
            {
                creation_year = "0";
            }
            std::uint16_t id_ = std::stoull(id);
            if (songs.contains(id_))
            {
                throw std::runtime_error("Duplicated song id " + id);
            }
            if (songs_name_map.contains(name))
            {
                throw std::runtime_error("Duplicated song name " + name);
            }
            if (status != "ADDED")
            {
                std::cout << "Skipping song \"" + name + "\", because its status is not ADDED." << std::endl;
                //SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Info", ("Skipping song \"" + name + "\", because its status is not ADDED.").data(), nullptr);
                continue;
            }
            const auto composers_ = split_string(composers,",",true);
            if (composers_.empty())
            {
                throw std::runtime_error("Song " + name + " has empty composers");
            }
            std::vector<std::uint16_t> composer_ids;
            for (const auto& composer : composers_)
            {
                if (!composers_name_map.contains(composer))
                {
                    throw std::runtime_error("Song " + name + " refers to non-existing composer: " + composer);
                }
                composer_ids.emplace_back(composers_name_map.at(composer));
            }
            SongBasicInfo song_info{};
            song_info.name = name;
            song_info.create_year = std::stoull(creation_year);
            song_info.description = description;
            song_info.maker = maker;
            song_info.composer_ids = composer_ids; 
            songs.emplace(id_, song_info);
            songs_name_map.emplace(name, id_);
        }
    }

    std::set<std::string> songs_in_db;
    std::cout << "doing song db" << std::endl;

    {
        io::CSVReader<5, io::trim_chars<' ', '\t'>, io::double_quote_escape<',', '"'>> csv("United Tiles Songs - Song Database.csv");
            csv.read_header(io::ignore_extra_column, "Song Name", "Filename", "Unlockable Type", "Unlockable Amount", "Leaderboard ID");
        std::string song_name, filename, unlockable_type, unlockable_amount, leaderboard_id;

        while (csv.read_row(song_name, filename, unlockable_type, unlockable_amount, leaderboard_id))
        {
            if (!songs_name_map.contains(song_name))
            {
                //SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Info", ("Skipping database song \"" + song_name + "\", because it doesn't exist.").data(), nullptr);
                continue;
            }
            if (songs_in_db.contains(song_name))
            {
                throw std::runtime_error("Duplicated database song " + song_name);
            }
            songs_in_db.emplace(song_name);
            auto& song = songs.at(songs_name_map.at(song_name));
            const auto get_unlockable = [&](std::string_view t_string){
                if (t_string.empty())
                {
                    return 0;
                }
                if (t_string == "Bronze")
                {
                    return 1;
                }
                if (t_string == "Silver")
                {
                    return 2;
                }
                if (t_string == "Gold")
                {
                    return 3;
                }
                if (t_string == "Diamond")
                {
                    return 4;
                }
                throw std::runtime_error("Invalid unlockable type: " + std::string(t_string));
            };
            song.unlockable_type = get_unlockable(unlockable_type);
            song.unlockable_amount = unlockable_amount.empty() ? 0 : std::stoull(unlockable_amount);
            song.leaderboard_id = leaderboard_id;
            const auto validate_filename = [&](std::string_view t_filename)
            {
                for (char c : t_filename)
                {
                        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')
                        || (c >= '0' && c <= '9') || c == ' ')
                    {

                    }
                    else
                    {
                        throw std::runtime_error("Filename "+std::string(t_filename)+" contains invalid char!");
                    }
                }
                if (!std::filesystem::exists("res/songs/" + std::string(t_filename) + ".usong"))
                {
                    throw std::runtime_error("Filename "+std::string(t_filename)+".usong doesn't exist!");
                }
            };
            validate_filename(filename);
            song.filename = filename;
        }
    }

    SongDatabase song_database;
    for (const auto& p : composers)
    {
        song_database.composers_infos.emplace_back(p);
    }
    for (const auto& p : songs)
    {
        song_database.songs_infos.emplace_back(p);
    }
    {
    std::ofstream file("songs.db", std::ios::binary | std::ios::trunc);
    song_database.to_file(file);
}
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Info", "Created songs.db file. Put it in res/database to test it.", nullptr);

}
catch(const std::exception& exception)
{
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", exception.what(), nullptr);
}