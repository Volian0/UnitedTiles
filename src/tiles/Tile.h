#pragma once

#include "../SongInfo.h"
#include "../Types.h"
#include "../Timepoint.h"

#include <cstdint>
#include <optional>
#include <memory>
#include <array>
#include <iostream>

enum TileColumn : uint8_t
{
	FAR_LEFT  = 0b11000000,
	MID_LEFT  = 0b00110000,
	MID_RIGHT = 0b00001100,
	FAR_RIGHT = 0b00000011,

	DT_LEFT   = 0b11001100,
	DT_RIGHT  = 0b00110011,

	ST_FAR_LEFT               = 0b01000000,
	ST_MID_LEFT_FACING_LEFT   = 0b00100000,
	ST_MID_LEFT_FACING_RIGHT  = 0b00010000,
	ST_MID_RIGHT_FACING_LEFT  = 0b00001000,
	ST_MID_RIGHT_FACING_RIGHT = 0b00000100,
	ST_FAR_RIGHT              = 0b00000010
};

inline constexpr std::array<TileColumn, 6> columns = {
	ST_FAR_RIGHT, ST_MID_RIGHT_FACING_LEFT, ST_MID_LEFT_FACING_LEFT,
	ST_FAR_LEFT, ST_MID_LEFT_FACING_RIGHT, ST_MID_RIGHT_FACING_RIGHT
};

[[nodiscard]] extern  Number get_column_x_pos(TileColumn column);
[[nodiscard]] extern TileColumn get_column(Number x_pos);
[[nodiscard]] extern TileColumn next_column(const std::shared_ptr<class Tile>& previous_tile);

struct ScoreInfo
{
	uint8_t points;
	bool silent = false;
};

enum class TileAction : uint8_t
{
	DELETE,
	NOTHING,
	GAME_OVER
};

struct TileStateInfo
{
	TileAction action;
	std::optional<ScoreInfo> score_info = {};
};

class Tile : NonCopyable
{
public:
	Tile(class StateLevel* level, TileColumn column_);
	virtual ~Tile() = default;

	const uint32_t id;
	const TileColumn column;

	[[nodiscard]] bool is_active() const;

	[[nodiscard]] Number get_tile_length() const;
	[[nodiscard]] Number get_height() const;

	[[nodiscard]] const TileInfo& get_info() const;

	virtual void render_bg() const;
	virtual void render_fg() const;

	[[nodiscard]] virtual TileAction get_action() = 0;

	void update();

	virtual void my_update();

	virtual void touch_down(uint16_t finger_id, Vec2 pos);
	virtual void touch_move(uint16_t finger_id, Vec2 pos);
	virtual void touch_up(uint16_t finger_id, Vec2 pos);

	[[nodiscard]] virtual bool should_game_over() const = 0;
	[[nodiscard]] virtual bool should_die() const = 0;

	TileColumn missed_column;

	Number y_offset;

protected:
	[[nodiscard]] bool force_first_interaction() const;
	void handle_first_interaction();
	void handle_clear();
	void handle_game_over();
	void handle_state_change(const TileStateInfo* state);
	StateLevel* _level;
};

template
<const TileStateInfo* default_state, const TileStateInfo* game_over_state, const TileStateInfo* cleared_state, const TileStateInfo* first_interaction_state>
class StatableTile : public Tile
{
public:
	inline StatableTile(class StateLevel* level, TileColumn column_)
		:Tile(level, column_)
	{
	}
	virtual ~StatableTile() = default;

	inline virtual void on_changed_state()
	{
	}

	inline bool change_state(const TileStateInfo* state)
	{
		if (_state == game_over_state)
		{
			return false;
		}
		if (state == first_interaction_state)
		{
			if (!is_active())
			{
				return false;
			}
			handle_first_interaction();
		}
		else if (force_first_interaction())
		{
			return false;
		}
		if (state == cleared_state)
		{
			handle_clear();
		}
		else if (state == game_over_state)
		{
			handle_game_over();
		}
		_state = state;
		handle_state_change(state);
		on_changed_state();
		return true;
	}

	[[nodiscard]] inline TileAction get_action() final
	{
		if (_state == game_over_state || _state->action == TileAction::NOTHING)
		{
			return TileAction::NOTHING;
		}
		if (_state->action == TileAction::GAME_OVER)
		{
			return should_game_over() ? TileAction::GAME_OVER : TileAction::NOTHING;
		}
		return should_die() ? TileAction::DELETE : TileAction::NOTHING;
	}

	[[nodiscard]] inline bool is_state(const TileStateInfo* state) const
	{
		return state == _state;
	}

private:
	const TileStateInfo* _state = default_state;
};