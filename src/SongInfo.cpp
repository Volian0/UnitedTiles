//TODO: implement to_file methods

#include "SongInfo.h"

#include "File.h"

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
	note_ticks_per_single_tile = read_u16(file).value();
	length_units_per_single_tile = read_u16(file).value();
	starting_tempo = read_number(file).value();
	uint32_t tiles_size = read_u32(file).value();
	for (uint32_t i = 0; i < tiles_size; ++i)
	{
		tiles.emplace_back(file);
	}
	uint32_t tempo_changes_size = read_u32(file).value();
	for (uint32_t i = 0; i < tempo_changes_size; ++i)
	{
		uint32_t tile_id = read_u32(file).value();
		tempo_changes.emplace(tile_id, read_number(file).value());
	}
}

void SongInfo::to_file(std::ofstream& file) const
{
	write_u16(file, version);
	write_u16(file, note_ticks_per_single_tile);
	write_u16(file, length_units_per_single_tile);
	write_number(file, starting_tempo);
	write_u32(file, tiles.size());
	for (const auto& tile : tiles)
	{
		tile.to_file(file);
	}
	write_u32(file, tempo_changes.size());
	for (const auto& [key, value] : tempo_changes)
	{
		write_u32(file, key);
		write_number(file, value);
	}
}

SongList::SongList(std::ifstream& file)
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
}
