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

class StateLevel;

class ScoreCounter : Unique
{
public:
	ScoreCounter(StateLevel* level_, uint32_t init_value = 0);
	
	void render() const;

	void set(uint32_t value);
	void add(uint32_t value);
	void silent_add(uint32_t value);
	void silent_update();

private:
	bool was_silent_added = false;
	StateLevel* _level;
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

	mutable Texture txt_long_tile;
	mutable Texture txt_long_tile_ext;
	mutable Texture txt_long_tile_clearing;
	mutable Texture txt_long_tile_circle;
	mutable Texture txt_long_tile_end;
	mutable Texture tile_divider;

	SongInfo _song_info;

	std::shared_ptr<Tile> previous_tile;
	Tile* game_over_tile = nullptr;

	uint32_t cleared_tiles = 0;

	void queue_notes(const std::multimap<uint32_t, NoteEvent>& notes, bool forceplay_old = true, const std::optional<Timepoint> custom_tp = {});

	Timepoint new_tp;

	std::optional<Number> game_over_scroll_to;

	ScoreCounter score;

	Number tps = 1.0L;

private:
	virtual void update();
	virtual void render() const;

	void restart_level();

	enum class State : uint8_t
	{
		IDLE, ACTIVE, GAME_OVER
	} _state = State::ACTIVE;

	//Number position = 0.0L;
	//uint32_t score = 0;
	Timepoint last_tempo_change;
	Number previous_position = 0;

	std::multimap<Number, std::shared_ptr<Tile>> tiles;
	uint32_t spawned_tiles = 0;

	DustMotes _dustmotes;
	DustMotes _dustmotes_stars;

	Timepoint _old_tp;

	mutable Texture bg;
	mutable Texture bg_o;

	Number _position = 0.0L;

	std::optional<Timepoint> game_over_reset;

	void change_tempo(Number new_tps, const Timepoint& tp_now, Number position);

	std::shared_ptr<Soundfont> soundfont;

	void game_over(Tile*);
};
