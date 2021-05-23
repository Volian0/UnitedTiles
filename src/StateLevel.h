#pragma once

#include "GameState.h"

#include "Font.h"
#include "Texture.h"
#include "FpsCounter.h"
#include "DevButton.h"
#include "Timepoint.h"
#include "DustMotes.h"
#include "Soundfont.h"
#include "Tile.h"

#include <memory>
#include <map>
#include <vector>

class StateLevel : public GameState
{
public:
	StateLevel(Game* game_, const std::string& filename);

private:
	virtual void update();
	virtual void render() const;

	SongInfo _song_info;

	enum class State : uint8_t
	{
		IDLE, ACTIVE, GAME_OVER
	} _state = State::ACTIVE;

	//Number position = 0.0L;
	uint32_t score = 0;
	Number tps = 1.0L;
	Timepoint last_tempo_change = Clock::now();
	Number previous_position = 0;

	std::multimap<Number, Tile> tiles;
	uint32_t cleared_tiles = 0;
	uint32_t spawned_tiles = 0;

	DustMotes _dustmotes;
	DustMotes _dustmotes_stars;

	Timepoint _old_tp = Clock::now();

	mutable Texture bg;
	mutable Texture txt_single_tile;
	mutable Texture tile_divider;
	mutable Texture bg_o;

	Number _position = 0.0L;

	void change_tempo(Number new_tps, const Timepoint& tp_now, Number position);

	std::shared_ptr<Soundfont> soundfont;
};
