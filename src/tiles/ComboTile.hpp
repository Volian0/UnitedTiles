#pragma once

#include "Tile.h"

inline constexpr auto ComboTileDefault         = TileStateInfo{ TileAction::GAME_OVER };
inline constexpr auto ComboTileMissed          = TileStateInfo{ TileAction::NOTHING };
inline constexpr auto ComboTileClearing        = TileStateInfo{ TileAction::NOTHING, ScoreInfo{0,true} };
inline constexpr auto ComboTileCleared    = TileStateInfo{ TileAction::DELETE,  ScoreInfo{0} };

class ComboTile : public StatableTile<&ComboTileDefault, &ComboTileMissed, &ComboTileCleared, &ComboTileClearing>
{
public:
	ComboTile(class StateLevel* level_);

private:
	void revive() override;

	[[nodiscard]] bool should_game_over() const final;
	[[nodiscard]] bool should_die() const final;
	void my_update() final;
	bool touch_down(uint16_t finger_id, Vec2 pos) final;
	void touch_up(  uint16_t finger_id, Vec2 pos) final;
	void on_changed_state() final;

	void render_fg() const final;

	std::vector<std::uint32_t> taps;
    std::uint32_t clicked_amount{0};
};