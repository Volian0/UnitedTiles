#pragma once

#include "Tile.h"

inline constexpr auto LongTileDefault         = TileStateInfo{ TileAction::GAME_OVER };
inline constexpr auto LongTileMissed          = TileStateInfo{ TileAction::NOTHING };
inline constexpr auto LongTileClearing        = TileStateInfo{ TileAction::NOTHING, ScoreInfo{1,true} };
inline constexpr auto LongTileNotFullyCleared = TileStateInfo{ TileAction::DELETE,  ScoreInfo{0} };
inline constexpr auto LongTileFullyCleared    = TileStateInfo{ TileAction::DELETE,  ScoreInfo{1} };

class LongTile : public StatableTile<&LongTileDefault, &LongTileMissed, &LongTileClearing, &LongTileClearing>
{
public:
	LongTile(class StateLevel* level_);

	static Number y_finger_tapped;

private:
	void revive() override;

	[[nodiscard]] bool should_game_over() const final;
	[[nodiscard]] bool should_die() const final;
	void my_update() final;
	bool touch_down(uint16_t finger_id, Vec2 pos) final;
	void touch_up(  uint16_t finger_id, Vec2 pos) final;
	void on_changed_state() final;

	void render_fg() const final;

	Number held_tile_duration = 0;
	Number tap_length = 0;

	std::vector<std::pair<Number, Number>> taps;

	FingerID finger{};
	Number y_tapped{};

	Number m_cleared_position{};
};