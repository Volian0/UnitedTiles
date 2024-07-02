//TODO: implement to_file methods

#include "SongInfo.h"

#include "Path.h"
#include "File.h"

#include <iostream>
#include <filesystem>
#include <algorithm>

NoteEvent::NoteEvent(Type type_, uint8_t key_, uint8_t velocity_)
	:type{type_},
	key{key_},
	velocity{velocity_}
{
}

NoteEvent::NoteEvent(std::ifstream& file)
{
	type = static_cast<Type>(read_u8(file).value());
	if (type == Type::ON || type == Type::OFF)
	{
		key = read_u8(file).value();
	}
	if (type == Type::ON)
	{
		velocity = read_u8(file).value();
	}
}

void NoteEvent::to_file(std::ofstream& file) const
{
	write_u8(file, static_cast<uint8_t>(type));
	if (type == Type::ON || type == Type::OFF)
	{
		write_u8(file, key);
	}
	if (type == Type::ON)
	{
		write_u8(file, velocity);
	}
}

TileInfo::TileInfo(std::ifstream& file)
{
	length = read_u32(file).value();
	type = static_cast<Type>(read_u8(file).value());
	if (type == Type::SINGLE || type == Type::LONG || type == Type::SLIDER || type == Type::DOUBLE)
	{
		uint32_t note_events_size = read_u32(file).value();
		for (uint32_t i = 0; i < note_events_size; ++i)
		{
			note_events.emplace(read_u32(file).value(), file);
		}
	}
	if (type == Type::DOUBLE)
	{
		uint32_t note_events2_size = read_u32(file).value();
		for (uint32_t i = 0; i < note_events2_size; ++i)
		{
			note_events_2nd_tile.emplace(read_u32(file).value(), file);
		}
	}
}

TileInfo::TileInfo(TileInfo&& tile_info) noexcept
{
	type = tile_info.type;
	length = tile_info.length;
	note_events = std::move(tile_info.note_events);
	note_events_2nd_tile = std::move(tile_info.note_events_2nd_tile);
}

void TileInfo::to_file(std::ofstream& file) const
{
	write_u32(file, length);
	write_u8(file, static_cast<uint8_t>(type));
	if (type == Type::SINGLE || type == Type::LONG || type == Type::SLIDER || type == Type::DOUBLE)
	{
		write_u32(file, note_events.size());
		for (const auto& [key, value] : note_events)
		{
			write_u32(file, key);
			value.to_file(file);
		}
	}
	if (type == Type::DOUBLE)
	{
		write_u32(file, note_events_2nd_tile.size());
		for (const auto& [key, value] : note_events_2nd_tile)
		{
			write_u32(file, key);
			value.to_file(file);
		}
	}
}

SongInfo::SongInfo(std::ifstream& file)
{
	version = read_u16(file).value();
	if (version != 1)
	{
		*this = std::move(legacy_song_load(version, file));
		return;
	}
	note_ticks_per_single_tile = read_u16(file).value();
	length_units_per_single_tile = read_u16(file).value();
	starting_tempo = read_number(file).value();
	acceleration_method = static_cast<AccelerationMethod>(read_u8(file).value());
	if (acceleration_method == AccelerationMethod::CLASSIC)
	{
		for (auto& tempo_change : acceleration_info.classic_tempo_changes)
		{
			auto tile_id = read_u32(file).value();
			auto TPS = read_number(file).value();
			tempo_change = std::make_pair<>(tile_id, TPS);
		}
	}
	else if (acceleration_method == AccelerationMethod::LINEAR)
	{
		uint32_t tempo_changes_size = read_u32(file).value();
		for (uint32_t i = 0; i < tempo_changes_size; ++i)
		{
			uint32_t tile_id = read_u32(file).value();
			acceleration_info.tempo_changes.emplace(tile_id, read_number(file).value());
		}
	}
	if (acceleration_method == AccelerationMethod::LINEAR || acceleration_method == AccelerationMethod::ACCELERATION)
	{
		acceleration_info.parameter = read_number(file).value();
	}
	uint32_t tiles_size = read_u32(file).value();
	for (uint32_t i = 0; i < tiles_size; ++i)
	{
		tiles.emplace_back(file);
	}
}

void SongInfo::to_file(std::ofstream& file) const
{
	write_u16(file, version);
	write_u16(file, note_ticks_per_single_tile);
	write_u16(file, length_units_per_single_tile);
	write_number(file, starting_tempo);
	write_u8(file, static_cast<uint8_t>(acceleration_method));
	if (acceleration_method == AccelerationMethod::CLASSIC)
	{
		for (const auto& tempo_change : acceleration_info.classic_tempo_changes)
		{
			write_u32(file, tempo_change.first);
			write_number(file, tempo_change.second);
		}
	}
	else if (acceleration_method == AccelerationMethod::LINEAR)
	{
		write_u32(file, acceleration_info.tempo_changes.size());
		for (const auto& [key, value] : acceleration_info.tempo_changes)
		{
			write_u32(file, key);
			write_number(file, value);
		}
	}
	if (acceleration_method == AccelerationMethod::LINEAR || acceleration_method == AccelerationMethod::ACCELERATION)
	{
		write_number(file, acceleration_info.parameter);
	}
	write_u32(file, tiles.size());
	for (const auto& tile : tiles)
	{
		tile.to_file(file);
	}
}

std::uint32_t SongInfo::calculate_perfect_score() const
{
	std::uint32_t score = 0;
	for (const auto& tile : tiles)
	{
		//SINGLE, LONG, DOUBLE, EMPTY, SLIDER
		switch(tile.type)
		{
		case TileInfo::Type::SINGLE:
			score += 1;
			break;
		case TileInfo::Type::SLIDER:
			score += 5;
			break;
		case TileInfo::Type::LONG:
		case TileInfo::Type::DOUBLE:
			score += 2;
			break;
		case TileInfo::Type::EMPTY:
			break;
		}
	}
	return score * 3;
}

/*SongList::SongList(std::ifstream& file)
{
	uint32_t song_size = read_u32(file).value();
	for (uint32_t i = 0; i < song_size; ++i)
	{
		std::string name     = read_string(file).value();
		std::string composer = read_string(file).value();
		std::string filename = read_string(file).value();
		songs.emplace_back(SongBasicInfo{name,composer,filename});
	}
}

void SongList::to_file(std::ofstream& file) const
{
	write_u32(file, songs.size());
	for (const auto& song : songs)
	{
		write_string(file, song.name);
		write_string(file, song.composer);
		write_string(file, song.filename);
	}
}*/

ComposerInfo::ComposerInfo(std::ifstream& file)
: name{*read_string(file)}, description{*read_string(file)},
birth_year{*read_u16(file)}, death_year{*read_u16(file)}
{
}

void ComposerInfo::to_file(std::ofstream& file) const
{
	write_string(file, name);
	write_string(file, description);
	write_u16(file, birth_year);
	write_u16(file, death_year);
}

SongBasicInfo::SongBasicInfo(std::ifstream& file)
:create_year{*read_u16(file)}, name{*read_string(file)}, description{*read_string(file)}, maker{*read_string(file)}, filename{*read_string(file)}
{
	const uint8_t composers_size = read_u8(file).value();
	for (uint8_t i = 0; i < composers_size; ++i)
	{
		composer_ids.emplace_back(read_u16(file).value());
	}
	unlockable_type = read_u8(file).value();
	unlockable_amount = read_u16(file).value();
	leaderboard_id = read_string(file).value();
}

void SongBasicInfo::to_file(std::ofstream& file) const
{
	write_u16(file, create_year);
	write_string(file, name);
	write_string(file, description);
	write_string(file, maker);
	write_string(file, filename);
	write_u8(file, composer_ids.size());
	for (auto composer_id : composer_ids)
	{
		write_u16(file, composer_id);
	}
	write_u8(file, unlockable_type);
	write_u16(file, unlockable_amount);
	write_string(file, leaderboard_id);
}

SongScore::SongScore(std::ifstream& file)
:reached_lap{*read_u32(file)}, reached_score{*read_u32(file)}
{

}

void SongScore::to_file(std::ofstream& file) const
{
	write_u32(file, reached_lap);
	write_u32(file, reached_score);
}

SongDatabase::SongDatabase(std::ifstream& file)
{
	const uint16_t composer_size = read_u16(file).value();
	for (uint16_t i = 0; i < composer_size; ++i)
	{
		const uint16_t composer_id = read_u16(file).value();
		composers_infos.emplace_back(composer_id, file);
	}

	const uint16_t song_size = read_u16(file).value();
	for (uint16_t i = 0; i < song_size; ++i)
	{
		const uint16_t song_id = read_u16(file).value();
		songs_infos.emplace_back(song_id, file);
	}
}

void SongDatabase::to_file(std::ofstream& file) const
{
	write_u16(file, composers_infos.size());
	for (const auto& [id, info] : composers_infos)
	{
		write_u16(file, id);
		info.to_file(file);
	}

	write_u16(file, songs_infos.size());
	for (const auto& [id, info] : songs_infos)
	{
		write_u16(file, id);
		info.to_file(file);
	}
}

SongUserDatabase::SongUserDatabase(std::ifstream& file)
{
	const uint16_t song_size = read_u16(file).value();
	for (uint16_t i = 0; i < song_size; ++i)
	{
		const auto id = read_u16(file).value();
		scores.emplace(id, file);
	}
}

[[deprecated]] void SongUserDatabase::to_file(std::ofstream& file) const
{
	write_u16(file, scores.size());
	for (const auto& [id, score] : scores)
	{
		write_u16(file, id);
		score.to_file(file);
	}
}

SongUserDatabase::SongUserDatabase()
{
	std::filesystem::create_directory(Path::user("","scores"));
}

void SongUserDatabase::load_from_files(bool t_load_legacy)
{
	for (const auto& entry : std::filesystem::directory_iterator(Path::user("","scores")))
	{
		const auto& path = entry.path();
		if (std::filesystem::is_regular_file(path) && path.extension() == ".scr")
		{
			const std::string stem = path.stem().string();
			if (stem.size() >= 1 && stem.size() <= 5 && std::all_of(stem.begin(), stem.end(), ::isdigit))
			{
				std::uint32_t song_id = std::stoull(stem);
				if (song_id <= 0xFFFF)
				{
					auto file = open_ifile(path.string());
					if (!file)
						continue;
					auto reached_lap = read_u32(*file);
					auto reached_score = read_u32(*file);
					auto perfect_score = read_u8(*file);
					file->close();
					if (file.value() && reached_lap && reached_score && perfect_score)
					{
						SongScore score;
						score.reached_lap = reached_lap.value();
						score.reached_score = reached_score.value();
						score.got_perfect_score = perfect_score.value();
						scores.emplace(song_id, score);
					}
				}
			}
		}
	}
	if (t_load_legacy)
	{
		SongUserDatabase db;
		db.load_from_file();
		for (const auto& [id, score] : db.scores)
		{
			if (scores.count(id))
			{
				auto& other_score = scores.at(id);
				if (other_score.reached_lap < score.reached_lap)
					other_score.reached_lap = score.reached_lap;
				if (other_score.reached_score < score.reached_score)
					other_score.reached_score = score.reached_score;
			}
			else
			{
				scores.emplace(id, score);
			}
		}
	}
}

void SongUserDatabase::update_score(uint16_t song_id, uint32_t new_lap, uint32_t new_score)
{
	auto& score = scores[song_id];
	if (new_lap <= score.reached_lap && new_score <= score.reached_score)
	{
		return;
	}
	if (score.reached_lap < new_lap)
	score.reached_lap = new_lap;
	if (score.reached_score < new_score)
	score.reached_score = new_score;
	{
		auto file = open_ofile(Path::user("temp_scores.db"));
		to_file(file.value());
		file->close();
		if (!file.value())
		{
			std::cout << "error with score!" << std::endl;
			std::abort();
		}
	}
	std::filesystem::rename(Path::user("temp_scores.db"), Path::user("scores.db"));
}

void SongUserDatabase::update_score(uint16_t song_id, uint32_t new_lap, uint32_t new_score, bool new_perfect_score)
{
	auto& score = scores[song_id];
	if (new_lap <= score.reached_lap && new_score <= score.reached_score && !new_perfect_score)
	{
		return;
	}
	if (score.reached_lap < new_lap)
		score.reached_lap = new_lap;
	if (score.reached_score < new_score)
		score.reached_score = new_score;
	if (new_perfect_score)
		score.got_perfect_score = true;

	const std::string tmp_path  = Path::user(std::to_string(song_id) + ".tmp","scores");
	auto file = open_ofile(tmp_path);
	write_u32(file.value(), score.reached_lap);
	write_u32(file.value(), score.reached_score);
	write_u8(file.value(), score.got_perfect_score ? 1 : 0);
	file->close();
	if (!file.value())
	{
		std::cout << "Error with saving score!" << std::endl;
		std::abort();
		return;
	}
	const std::string dest_path = Path::user(std::to_string(song_id) + ".scr","scores");
	std::filesystem::rename(tmp_path, dest_path);
}

[[deprecated]] void SongUserDatabase::load_from_file()
{
	const auto& path = Path::user("scores.db");
	auto file = open_ifile(path);
	if (!file)
	{
		if (std::filesystem::is_regular_file(path))
		{
			std::cout << "Error with opening score file!" << std::endl;
			std::abort();
		}
		return;
	}
	*this = file.value();
}

SongInfo legacy_song_load(uint16_t version, std::ifstream& file)
{
	SongInfo song_info;
	song_info.version = version;
	if (version == 0)
	{
		song_info.acceleration_method = SongInfo::AccelerationMethod::LINEAR;
		song_info.acceleration_info.parameter = 1.25L;

		song_info.note_ticks_per_single_tile = read_u16(file).value();
		song_info.length_units_per_single_tile = read_u16(file).value();
		song_info.starting_tempo = read_number(file).value();
		uint32_t tiles_size = read_u32(file).value();
		for (uint32_t i = 0; i < tiles_size; ++i)
		{
			song_info.tiles.emplace_back(file);
		}
		uint32_t tempo_changes_size = read_u32(file).value();
		for (uint32_t i = 0; i < tempo_changes_size; ++i)
		{
			uint32_t tile_id = read_u32(file).value();
			song_info.acceleration_info.tempo_changes.emplace(tile_id, read_number(file).value());
		}
	}
	else
	{
		abort();
	}
	return song_info;
}
