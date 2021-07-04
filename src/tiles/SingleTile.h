#include "Tile.h"

inline constexpr auto SingleTileDefault = TileStateInfo{ TileAction::GAME_OVER };
inline constexpr auto SingleTileMissed = TileStateInfo{ TileAction::NOTHING };
inline constexpr auto SingleTileCleared = TileStateInfo{ TileAction::DELETE, ScoreInfo{1} };

class SingleTile final : public StatableTile<&SingleTileDefault, &SingleTileMissed, &SingleTileCleared, &SingleTileCleared>
{
public:
	SingleTile(class StateLevel* level_);

private:
	bool should_game_over() const final;
	bool should_die() const final;
	void touch_down(uint16_t finger_id, Vec2 pos) final;
	void on_changed_state() final;

	void render_fg() const final;
};