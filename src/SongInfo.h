#pragma once

#include "Unique.h"
#include "Types.h"

#include <cstdint>
#include <vector>
#include <map>
#include <fstream>
#include <array>

struct NoteEvent
{
	enum class Type : uint8_t {
		ON, OFF, ALL_OFF
	} type;
	uint8_t key, velocity;

	NoteEvent() = default;
	NoteEvent(Type type_, uint8_t key_ = 0, uint8_t velocity_ = 0);
	NoteEvent(std::ifstream& file);
	void to_file(std::ofstream& file) const;
};

struct TileInfo : Unique
{
	uint32_t length;
	enum class Type : uint8_t {
		SINGLE, LONG, DOUBLE,
		EMPTY, SLIDER
	} type;
	std::multimap<uint32_t, NoteEvent> note_events;
	std::multimap<uint32_t, NoteEvent> note_events_2nd_tile;

	TileInfo() = default;
	TileInfo(std::ifstream& file);
	TileInfo(TileInfo&& tile_info) noexcept;
	void to_file(std::ofstream& file) const;
};

struct AccelerationInfo
{
	std::array<std::pair<uint32_t, Number>, 2> classic_tempo_changes;
	std::map<uint32_t, Number> tempo_changes;
	Number parameter; //acceleration value or multiplier
};

struct SongInfo
{
	uint16_t version;
	uint16_t note_ticks_per_single_tile;
	uint16_t length_units_per_single_tile;
	Number starting_tempo;
	enum class AccelerationMethod : uint8_t {
		CONSTANT = 0, CLASSIC, LINEAR, ACCELERATION
	} acceleration_method;
	AccelerationInfo acceleration_info;
	std::vector<TileInfo> tiles;

	SongInfo() = default;
	SongInfo(std::ifstream& file);
	void to_file(std::ofstream& file) const;
};

SongInfo legacy_song_load(uint16_t version, std::ifstream& file);

struct SongBasicInfo
{
	std::string name, composer, filename;
};

struct SongList
{
	std::vector<SongBasicInfo> songs;
	SongList() = default;
	SongList(std::ifstream& file);
	void to_file(std::ofstream& file) const;
};