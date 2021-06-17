#pragma once

#include "Tile.h"

inline constexpr auto SliderTileDefault = TileStateInfo{ TileAction::GAME_OVER };
inline constexpr auto SliderTileClearing = TileStateInfo{ TileAction::NOTHING };
inline constexpr auto SliderTileMissed = TileStateInfo{ TileAction::NOTHING };
inline constexpr auto SliderTileCleared = TileStateInfo{ TileAction::DELETE, ScoreInfo{1} };

class SliderTile : public StatableTile<&SliderTileDefault, &SliderTileMissed, &SliderTileCleared, &SliderTileClearing>
{
public:
	SliderTile(class StateLevel* level_);

	std::pair<TileColumn, std::optional<TileColumn>> get_end_column() const;

private:
	bool should_game_over(Number y_offset) const final;
	bool should_die(Number y_offset) const final;
	void my_update(Number y_offset, bool force_first_interaction = false) final;
	void touch_down(uint16_t finger_id, Vec2 pos, bool force_first_interaction = false) final;
	void touch_move(uint16_t finger_id, Vec2 pos, bool force_first_interaction = false) final;
	void touch_up(uint16_t finger_id, Vec2 pos, bool force_first_interaction = false) final;
	void on_changed_state() final;

	void render_fg(Number y_offset) const final;

	Number get_x_pos(Number y_offset) const;
	//bool is_facing_left(Number y_offset) const;
	TileColumn next_column(const std::shared_ptr<Tile>& previous_tile) const;
	bool in_range(Vec2 pos) const;

	Number held_tile_duration = 0;

	uint8_t get_column_index() const;
	const uint8_t _column_index;

	void debug_draw_hitbox(Number y_offset) const;

	void render_fragment(class Texture* texture, Number length, Vec2 pos, bool facing_right) const;

	FingerID finger;
	std::optional<Number> previous_offset;
	Vec2 finger_pos;
};