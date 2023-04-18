#pragma once

#include "NonCopyable.h"
#include "Types.h"

#include <cstdint>
#include <vector>
#include <map>
#include <fstream>
#include <array>
#include <set>

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

struct TileInfo : NonCopyable
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
	std::uint32_t calculate_perfect_score() const;
};

SongInfo legacy_song_load(uint16_t version, std::ifstream& file);

struct ComposerInfo
{
	std::string name;
	std::string description;
	uint16_t birth_year = 0;
	uint16_t death_year = 0;
	[[deprecated]] ComposerInfo(std::string a1, std::string a2, uint16_t a3, uint16_t a4)
	{
		name = a1;
		description = a2;
		birth_year = a3;
		death_year = a4;
	}
	ComposerInfo() = default;
	ComposerInfo(std::ifstream& file);
	void to_file(std::ofstream& file) const;
};

struct SongBasicInfo
{
	uint16_t create_year = 0;
	std::string name;
	std::string description;
	std::string maker;
	std::string filename;
	std::vector<uint16_t> composer_ids;
	uint8_t unlockable_type = 0; //0: free, 1: bronze, 2: silver, 3: gold, 4: diamond
	uint16_t unlockable_amount = 0;
	SongBasicInfo() = default;
	[[deprecated]] SongBasicInfo(uint16_t a1, std::string a2, std::string a3, std::string a4, std::vector<uint16_t> a5, uint8_t a6 = 0, uint16_t a7 = 0, const std::string& t_maker = {})
	{
		create_year = a1;
		name = a2;
		description = a3;
		maker = t_maker;
		filename = a4;
		composer_ids = a5;
		unlockable_type = a6;
		unlockable_amount = a7;
	}
	SongBasicInfo(std::ifstream& file);
	void to_file(std::ofstream& file) const;
};

struct SongScore
{
	uint32_t reached_lap = 0;
	uint32_t reached_score = 0;
	bool got_perfect_score = false; //3 laps completed (4th lap reached) and perfect score
	SongScore() = default;
	[[deprecated]] SongScore(std::ifstream& file);
	[[deprecated]] void to_file(std::ofstream& file) const;
};

struct SongDatabase
{
	std::vector<std::pair<uint16_t, ComposerInfo>> composers_infos;
	std::vector<std::pair<uint16_t, SongBasicInfo>> songs_infos;
	SongDatabase() = default;
	SongDatabase(std::ifstream& file);
	void to_file(std::ofstream& file) const;
};

struct SongUserDatabase
{
	std::map<uint16_t, SongScore> scores;
	SongUserDatabase();
	[[deprecated]] void load_from_file();
	void load_from_files(bool t_load_legacy = true);
	SongUserDatabase(std::ifstream& file);
	[[deprecated]] void to_file(std::ofstream& file) const;
	[[deprecated]] void update_score(uint16_t song_id, uint32_t new_lap, uint32_t new_score);
	void update_score(uint16_t song_id, uint32_t new_lap, uint32_t new_score, bool new_perfect_score);
};