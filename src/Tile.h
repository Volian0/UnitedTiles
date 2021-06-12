#pragma once

#include "SongInfo.h"
#include "Types.h"
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
	DT_RIGHT  = 0b00110011,
};

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
	virtual bool touch_down(uint16_t finger_id, Vec2 pos);
	virtual bool touch_up(uint16_t finger_id, Vec2 pos);
	virtual bool touch_move(uint16_t finger_id, Vec2 pos);

//protected:
	TileInfo* info;
	class StateLevel* level;
};

class SingleTile final : public Tile
{
public:
	SingleTile(uint32_t tile_id_, class StateLevel* level_);

	static TileColumn next_column(const std::shared_ptr<Tile>& previous_tile);
	static Number get_x_pos(TileColumn column_);

private:
	bool is_cleared() const override;
	void render(Number y_offset) const override;
	bool should_be_cleared(Number y_offset) const override;
	bool should_die(Number y_offset) const override;
	bool touch_down(uint16_t finger_id, Vec2 pos) override;

	std::optional<TileColumn> game_over_column;
	bool _cleared = false;
};

class LongTile final : public Tile
{
public:
	LongTile(uint32_t tile_id_, class StateLevel* level_);

private:
	bool is_cleared() const override;
	void render(Number y_offset) const override;
	bool should_be_cleared(Number y_offset) const override;
	bool should_die(Number y_offset) const override;
	bool touch_down(uint16_t finger_id, Vec2 pos) override;
	bool touch_up(uint16_t finger_id, Vec2 pos) override;

	std::optional<TileColumn> game_over_column;
	std::optional<std::pair<uint16_t, Timepoint>> finger_id_clicked;
	mutable bool _is_held = false;

	mutable Number held_tile_duration = 0.0L;

	mutable bool fully_cleared = false;
};

class DoubleTile final : public Tile
{
public:
	DoubleTile(uint32_t tile_id_, class StateLevel* level_);

private:
	bool is_cleared() const override;
	void render(Number y_offset) const override;
	bool should_be_cleared(Number y_offset) const override;
	bool should_die(Number y_offset) const override;
	bool touch_down(uint16_t finger_id, Vec2 pos) override;

	TileColumn next_column(const std::shared_ptr<Tile>& previous_tile);

	std::optional<TileColumn> game_over_column;
	bool _cleared_left = false;
	bool _cleared_right = false;

	std::optional<Timepoint> clicked;
};

class EmptyTile final : public Tile
{
public:
	EmptyTile(uint32_t tile_id_, class StateLevel* level_);

	std::shared_ptr<Tile> previous_tile;

private:
	bool is_cleared() const override;
	void render(Number y_offset) const override;
	bool should_be_cleared(Number y_offset) const override;
	bool should_die(Number y_offset) const override;
	bool touch_down(uint16_t finger_id, Vec2 pos) override;

	std::optional<TileColumn> game_over_column;
	mutable bool _cleared = false;
};

struct SliderTileLine
{
	uint32_t length;
	TileColumn column;
	bool is_facing_left;
	//uint32_t starting_note_tick;
	Number progress = 0;
	std::optional<Timepoint> cleared;

	void render(Number y_offset, const class SliderTile* tile) const;
	bool is_cleared() const;
	void update_pos(Vec2 pos, class SliderTile* tile);
};

class SliderTile final : public Tile
{
public:
	SliderTile(uint32_t tile_id_, class StateLevel* level_);

private:
	bool is_cleared() const override;
	void render(Number y_offset) const override;
	bool should_be_cleared(Number y_offset) const override;
	bool should_die(Number y_offset) const override;
	bool touch_down(uint16_t finger_id, Vec2 pos) override;
	bool touch_up(uint16_t finger_id, Vec2 pos) override;
	bool touch_move(uint16_t finger_id, Vec2 pos) override;

	uint32_t get_cleared_lines() const;

	std::optional<std::pair<uint16_t,Vec2>> finger;
	bool is_held = false;
	std::vector<SliderTileLine> lines;
};