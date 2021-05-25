#pragma once

#include "SongInfo.h"
#include "Number.h"
#include "Timepoint.h"

#include <cstdint>
#include <optional>
#include <memory>

enum TileColumn : uint8_t
{
	FAR_LEFT  = 0b11000000,
	MID_LEFT  = 0b00110000,
	MID_RIGHT = 0b00001100,
	FAR_RIGHT = 0b00000011,
	DT_LEFT   = 0b11001100,
	DR_RIGHT  = 0b00110011
};

/*struct Tile
{
	uint32_t tile_id;
	TileColumn column;
	bool cleared = false;
	std::optional<Timepoint> tp;
	std::optional<uint16_t> finger_id;
};*/

class Tile : Unique
{
public:
	Tile(uint32_t tile_id_, class StateLevel* level_, TileColumn column_);
	virtual ~Tile() = default;

	const uint32_t tile_id;
	const TileColumn column;

	virtual bool is_cleared() const = 0;
	virtual void render(Number y_offset) const = 0;
	//virtual TileColumn next_column(const std::shared_ptr<Tile>& previous_tile) const = 0;
	bool active() const;

	virtual bool should_be_cleared(Number y_offset) const = 0;
	virtual bool should_die(Number y_offset) const = 0;

	//return false to signalize game over; the pos is relative
	virtual bool touch_down(uint16_t finger_id, Vec2 pos) = 0;

//protected:
	TileInfo* info;
	class StateLevel* level;
};

class SingleTile : public Tile
{
public:
	SingleTile(uint32_t tile_id_, class StateLevel* level_);

private:
	bool is_cleared() const override;
	void render(Number y_offset) const override;
	static TileColumn next_column(const std::shared_ptr<Tile>& previous_tile);
	static Number get_x_pos(TileColumn column_);
	bool should_be_cleared(Number y_offset) const override;
	bool should_die(Number y_offset) const override;
	bool touch_down(uint16_t finger_id, Vec2 pos) override;

	std::optional<TileColumn> game_over_column;
	bool _cleared = false;
};