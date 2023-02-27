#pragma once

#include "GameState.h"

#include "Font.h"
#include "Texture.h"
#include "FpsCounter.h"
#include "DevButton.h"
#include "Timepoint.h"
#include "DustMotes.h"
#include "Soundfont.h"
#include "tiles/Tile.h"
#include "BurstParticles.h"

#include <memory>
#include <map>
#include <vector>
#include <cstdint>
#include <array>
#include <deque>

class StateLevel;

class LevelBackground : NonCopyable
{
	public:
		LevelBackground(StateLevel* level);
		void update(Number delta_time);

		void change_state(std::uint8_t new_state);
		void render() const;

		[[nodiscard]] constexpr std::uint8_t get_state() const noexcept
		{
			return _state;
		}

	private:
		StateLevel* _level;
		Number _alpha_second = 0.0L;
		Number _alpha_third = 0.0L;
		std::uint8_t _state = 0;
		DustMotes _dustmotes;
		DustMotes _dustmotes_mini;
		DustMotes _dustmotes_stars;
		mutable std::array<Texture, 3> backgrounds;
};

class ScoreCounter : NonCopyable
{
public:
	ScoreCounter(StateLevel* level_, uint32_t init_value = 0);
	
	void render() const;

	void set(uint32_t value);
	void add(uint32_t value);
	void silent_add(uint32_t value);
	void silent_update();

	[[nodiscard]] constexpr uint32_t get_score() const noexcept
	{
		return _value;
	}

	bool show_tps = false;

private:
	bool was_silent_added = false;
	StateLevel* _level;
	mutable Font _font;
	mutable std::unique_ptr<Texture> _texture;
	mutable uint32_t _value;
	mutable Timepoint _tp_update;
	mutable std::uint8_t _odd = 0;
	mutable std::string old_tps;
};

class StateLevel : public GameState
{
	friend Tile;

public:
	StateLevel(Game* game_, uint16_t song_id_);

	const uint16_t song_id;

	mutable Texture txt_single_tile;
	mutable Texture txt_game_over_tile;
	mutable Texture txt_single_tile_cleared;

	mutable Texture txt_long_tile;
	mutable Texture txt_long_tile_ext;
	mutable Texture txt_long_tile_clearing;
	mutable Texture txt_long_tile_circle;
	mutable Texture txt_long_tile_end;
	mutable Texture tile_divider;
	mutable Texture slider_tile;
	mutable Texture slider_tile_clearing;
	mutable Texture txt_white;
	mutable std::optional<Texture> txt_arrow;

	Color theme_tint = Colors::CYAN;

	uint32_t lap_id = 0;

	//std::vector<std::pair<Number,Number>> long_tile_taps;
	//std::pair<Number,Number> current_long_tile_tap{0.0L,0.0L};
	//class LongTile* tapped_long_tile=nullptr;

	//std::vector<std::pair<uint16_t,Vec2>> touch_down_sorted_positions;

	SongInfo _song_info;

	std::shared_ptr<Tile> previous_tile;
	Tile* game_over_tile = nullptr;

	mutable Font _debug_font;

	void render_debug() const;

	uint64_t cleared_tiles = 0;

	void queue_notes(const std::multimap<uint32_t, NoteEvent>& notes, bool forceplay_old = true, const std::optional<Timepoint> custom_tp = {});

	[[nodiscard]] bool force_first_interaction() const;

	Timepoint new_tp;

	std::optional<Number> game_over_scroll_to;

	ScoreCounter score;

	Number tps = 1.0L;

	constexpr Number get_miss_range() const noexcept
	{
		return tps * 0.1171875L;
	}

	BurstParticles _burst;

	[[nodiscard]] Number get_tile_pos(const Tile* tile_);

	Number _position = 0.0L;

	uint64_t total_length = 0;

	LevelBackground lv_bg;

	void set_theme_tint(Color t_tint);

private:
	virtual void update();
	virtual void render() const;

	void restart_level();

	void spawn_new_tiles();

	//bool _started = false;

	enum class State : uint8_t
	{
		IDLE, ACTIVE, GAME_OVER
	} _state = State::IDLE;

	Timepoint tp_state_start;

	Timepoint last_tempo_change;
	Number previous_position = 0;

	std::deque<std::pair<Number, std::shared_ptr<Tile>>> tiles;
	uint32_t spawned_tiles = 0;

	Timepoint _old_tp;

	std::optional<Timepoint> game_over_reset;

	void change_tempo(Number new_tps, const Timepoint& tp_now, Number position);

	std::shared_ptr<Soundfont> soundfont;

	void game_over(Tile*);
};
