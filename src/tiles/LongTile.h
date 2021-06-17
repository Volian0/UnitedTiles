#pragma once

#include "Tile.h"

inline constexpr auto LongTileDefault         = TileStateInfo{ TileAction::GAME_OVER };
inline constexpr auto LongTileMissed          = TileStateInfo{ TileAction::NOTHING };
inline constexpr auto LongTileClearing       = TileStateInfo{ TileAction::NOTHING, ScoreInfo{1,true} };
inline constexpr auto LongTileNotFullyCleared = TileStateInfo{ TileAction::DELETE, ScoreInfo{0} };
inline constexpr auto LongTileFullyCleared    = TileStateInfo{ TileAction::DELETE, ScoreInfo{1} };

class LongTile : public StatableTile<&LongTileDefault, &LongTileMissed, &LongTileClearing, &LongTileClearing>
{
public:
	LongTile(class StateLevel* level_);

private:
	bool should_game_over(Number y_offset) const final;
	bool should_die(Number y_offset) const final;
	void my_update(Number y_offset, bool force_first_interaction = false) final;
	void touch_down(uint16_t finger_id, Vec2 pos, bool force_first_interaction = false) final;
	void touch_up(  uint16_t finger_id, Vec2 pos, bool force_first_interaction = false) final;
	void on_changed_state() final;

	void render_fg(Number y_offset) const final;

	Number held_tile_duration = 0;

	FingerID finger;
	Timepoint tp_tapped;
};