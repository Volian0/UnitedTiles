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
#include "ui/Label.h"

#include <memory>
#include <map>
#include <vector>
#include <cstdint>
#include <array>
#include <deque>
#include <string_view>

class StateLevel;

inline constexpr bool halloween_theme = false;
#ifdef __ANDROID__
inline constexpr bool pc_controls = false;
#else
inline constexpr bool pc_controls = true;
#endif

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
	Vec2 offset{0.0L};
	Number scale{1.0L};
	bool is_auto = false;

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
	StateLevel(Game* game_, uint16_t song_id_, std::string_view t_song_filename = {}, bool t_from_res = false);



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

	mutable std::optional<Label> lbl_maker;

	mutable std::optional<Texture> txt_trail;

	Color theme_tint = Colors::CYAN;

	uint32_t lap_id = 0;

	bool perfect_score = false;

	//std::vector<std::pair<Number,Number>> long_tile_taps;
	//std::pair<Number,Number> current_long_tile_tap{0.0L,0.0L};
	//class LongTile* tapped_long_tile=nullptr;

	//std::vector<std::pair<uint16_t,Vec2>> touch_down_sorted_positions;

	SongInfo _song_info;

	std::shared_ptr<Tile> previous_tile;
	Tile* game_over_tile = nullptr;

	mutable Font _debug_font;

	mutable Texture _dev_button_texture;
	std::optional<Timepoint> revive_tp;
	DevButton revive_yes;
	DevButton revive_no;

	[[nodiscard]] constexpr std::optional<unsigned> get_revive_seconds() const noexcept
	{
		if (!revive_tp)
		{
			return std::nullopt;
		}
		unsigned seconds = new_tp - *revive_tp;
		if (seconds >= 60)
		{
			return std::nullopt;
		}
		return 60 - seconds;
	}

	std::optional<TileColumn> dt_column;

	void render_debug() const;

	uint64_t cleared_tiles = 0;

	void queue_notes(const std::multimap<uint32_t, NoteEvent>& notes, bool forceplay_old = true, const std::optional<Timepoint> custom_tp = {});

	[[nodiscard]] bool force_first_interaction() const;

	Timepoint new_tp;

	std::optional<Number> game_over_scroll_to;

	ScoreCounter score;
	ScoreCounter score_tps;

	Number tps = 1.0L;

	bool can_be_revived = true;

	Number get_miss_range() const noexcept;

	BurstParticles _burst;

	[[nodiscard]] Number get_tile_pos(const Tile* tile_);

	Number _position = 0.0L;

	uint64_t total_length = 0;

	LevelBackground lv_bg;

	void set_theme_tint(Color t_tint);

	[[nodiscard]] auto is_game_over() const -> bool
	{
		return _state == State::GAME_OVER;
	}
	bool save_score = true;

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

	std::string scoreboard_id;

	void game_over(Tile*);

	std::vector<std::unique_ptr<Texture>> pc_keys;
};
