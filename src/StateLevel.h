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
#include <cstdint>

class ScoreCounter : Unique
{
public:
	ScoreCounter(Renderer* renderer_, uint32_t init_value = 0);
	
	void render() const;

	void set(uint32_t value);
	void add(uint32_t value);

private:
	Renderer* _renderer;
	Font _font;
	std::unique_ptr<Texture> _texture;
	uint32_t _value;
	Timepoint _tp_update;
};

class StateLevel : public GameState
{
	friend Tile;

public:
	StateLevel(Game* game_, const std::string& filename);

	mutable Texture txt_single_tile;
	mutable Texture txt_game_over_tile;
	mutable Texture txt_single_tile_cleared;

	SongInfo _song_info;

	std::shared_ptr<Tile> previous_tile;

	uint32_t cleared_tiles = 0;

	void queue_notes(const std::multimap<uint32_t, NoteEvent>& notes, bool forceplay_old = true);

	Timepoint new_tp = Clock::now();

	ScoreCounter score;

private:
	virtual void update();
	virtual void render() const;

	enum class State : uint8_t
	{
		IDLE, ACTIVE, GAME_OVER
	} _state = State::ACTIVE;

	//Number position = 0.0L;
	//uint32_t score = 0;
	Number tps = 1.0L;
	Timepoint last_tempo_change = Clock::now();
	Number previous_position = 0;

	std::multimap<Number, std::shared_ptr<Tile>> tiles;
	uint32_t spawned_tiles = 0;

	DustMotes _dustmotes;
	DustMotes _dustmotes_stars;

	Timepoint _old_tp = Clock::now();

	mutable Texture bg;
	mutable Texture tile_divider;
	mutable Texture bg_o;

	Number _position = 0.0L;

	void change_tempo(Number new_tps, const Timepoint& tp_now, Number position);

	std::shared_ptr<Soundfont> soundfont;

	void game_over(Tile*);
};
