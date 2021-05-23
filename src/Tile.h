#pragma once

#include "SongInfo.h"
#include "Number.h"
#include "Timepoint.h"

#include <cstdint>
#include <optional>

enum TileColumn : uint8_t
{
	FAR_LEFT  = 0b11000000,
	MID_LEFT  = 0b00110000,
	MID_RIGHT = 0b00001100,
	FAR_RIGHT = 0b00000011,
	DT_LEFT   = 0b11001100,
	DR_RIGHT  = 0b00110011
};

struct Tile
{
	uint32_t tile_id;
	TileColumn column;
	bool cleared = false;
	std::optional<Timepoint> tp;
	std::optional<uint16_t> finger_id;
};