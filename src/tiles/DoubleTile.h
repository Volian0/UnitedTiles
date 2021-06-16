#pragma once

#include "Tile.h"

inline constexpr auto DoubleTileDefault = TileStateInfo{ TileAction::GAME_OVER };
inline constexpr auto DoubleTileMissed = TileStateInfo{ TileAction::NOTHING };
inline constexpr auto DoubleTilePartiallyCleared = TileStateInfo{ TileAction::GAME_OVER, ScoreInfo{1} };
inline constexpr auto DoubleTileFullyCleared = TileStateInfo{ TileAction::DELETE, ScoreInfo{1} };

class DoubleTile : public StatableTile<&DoubleTileDefault, &DoubleTileMissed, &DoubleTileFullyCleared, &DoubleTilePartiallyCleared>
{
public:
	DoubleTile(class StateLevel* level_);

private:
	bool should_game_over(Number y_offset) const final;
	bool should_die(Number y_offset) const final;
	void touch_down(uint16_t finger_id, Vec2 pos, bool force_first_interaction = false) final;
	void on_changed_state() final;

	void render_bg(Number y_offset) const final;
	void render_fg(Number y_offset) const final;

	TileColumn next_column(const std::shared_ptr<Tile>& previous_tile);

	std::optional<Timepoint> tp_tapped;
	bool left_tile_cleared;
};