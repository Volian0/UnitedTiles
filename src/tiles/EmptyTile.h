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
	[[nodiscard]] bool should_game_over() const final;
	[[nodiscard]] bool should_die() const final;
	bool touch_down(uint16_t finger_id, Vec2 pos) final;
	void my_update() final;

	void render_fg() const final;
};