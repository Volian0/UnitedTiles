#pragma once

#include "Tile.h"

inline constexpr auto EmptyTileDefault = TileStateInfo{TileAction::NOTHING};
inline constexpr auto EmptyTileMissed = TileStateInfo{ TileAction::NOTHING };
inline constexpr auto EmptyTileCleared = TileStateInfo{ TileAction::DELETE };

class EmptyTile final : public StatableTile<&EmptyTileDefault, &EmptyTileMissed, &EmptyTileCleared, &EmptyTileCleared>
{
public:
	EmptyTile(class StateLevel* level_);

	std::shared_ptr<Tile> previous_tile;

private:
	bool should_game_over(Number y_offset) const final;
	bool should_die(Number y_offset) const final;
	void touch_down(uint16_t finger_id, Vec2 pos, bool force_first_interaction = false) final;
	void my_update(Number y_offset, bool force_first_interaction = false) final;

	void render_fg(Number y_offset) const final;
};