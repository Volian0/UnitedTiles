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

extern Number get_column_x_pos(TileColumn column);
extern TileColumn get_column(Number x_pos);
extern TileColumn next_column(const std::shared_ptr<class Tile>& previous_tile);

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

class Tile : Unique
{
public:
	Tile(class StateLevel* level, TileColumn column_);
	virtual ~Tile() = default;

	const uint32_t id;
	const TileColumn column;

	bool is_active() const;

	Number get_tile_length() const;
	Number get_height() const;

	const TileInfo& get_info() const;

	virtual void render_bg(Number y_offset) const;
	virtual void render_fg(Number y_offset) const;

	virtual TileAction get_action(Number y_offset) = 0;

	void update(Number y_offset, bool force_first_interaction = false);

	virtual void my_update(Number y_offset, bool force_first_interaction = false);

	virtual void touch_down(uint16_t finger_id, Vec2 pos, bool force_first_interaction = false);
	virtual void touch_move(uint16_t finger_id, Vec2 pos, bool force_first_interaction = false);
	virtual void touch_up(uint16_t finger_id, Vec2 pos, bool force_first_interaction = false);

	virtual bool should_game_over(Number y_offset) const = 0;
	virtual bool should_die(Number y_offset) const = 0;

	TileColumn missed_column;

protected:
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

	inline bool change_state(const TileStateInfo* state, bool force_first_interaction = false)
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
		else if (force_first_interaction)
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

	inline TileAction get_action(Number y_offset) final
	{
		if (_state == game_over_state || _state->action == TileAction::NOTHING)
		{
			return TileAction::NOTHING;
		}
		if (_state->action == TileAction::GAME_OVER)
		{
			return should_game_over(y_offset) ? TileAction::GAME_OVER : TileAction::NOTHING;
		}
		return should_die(y_offset) ? TileAction::DELETE : TileAction::NOTHING;
	}

	inline bool is_state(const TileStateInfo* state) const
	{
		return state == _state;
	}

private:
	const TileStateInfo* _state = default_state;
};