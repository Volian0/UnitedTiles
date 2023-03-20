#pragma once

#include "Tile.h"

#include <array>

inline constexpr auto DoubleTileDefault = TileStateInfo{ TileAction::GAME_OVER };
inline constexpr auto DoubleTileMissed = TileStateInfo{ TileAction::NOTHING };
inline constexpr auto DoubleTilePartiallyCleared = TileStateInfo{ TileAction::GAME_OVER, ScoreInfo{1} };
inline constexpr auto DoubleTileFullyCleared = TileStateInfo{ TileAction::DELETE, ScoreInfo{1} };

class DoubleTile : public StatableTile<&DoubleTileDefault, &DoubleTileMissed, &DoubleTileFullyCleared, &DoubleTilePartiallyCleared>
{
public:
	DoubleTile(class StateLevel* level_);

private:
	[[nodiscard]] bool should_game_over() const final;
	[[nodiscard]] bool should_die() const final;
	void touch_down(uint16_t finger_id, Vec2 pos) final;
	void on_changed_state() final;

	void render_bg() const final;
	void render_fg() const final;

	[[nodiscard]] TileColumn next_column(const std::shared_ptr<Tile>& previous_tile);

	std::optional<Timepoint> tp_tapped;
	std::optional<Timepoint> tp_second;
	bool left_tile_cleared;

	std::array<Number, 2> m_cleared_positions;
};