#include "Tile.h"

#include "../StateLevel.h"
#include "../Game.h"
#include "../AudioDevice.h"
#include "../Renderer.h"
#include "../RNG.h"
#include "../Colors.h"
#include "EmptyTile.h"
#include "SliderTile.h"

#include <iostream>
#include <algorithm>
#include <unordered_set>

Tile::Tile(StateLevel* level, TileColumn column_)
	:_level{level},
	id{ level->spawned_tiles % level->_song_info.tiles.size() },
	column{column_}
{
}

Number get_column_x_pos(TileColumn column)
{
	if (column & FAR_LEFT)
		return -0.75;
	if (column & MID_LEFT)
		return -0.25;
	if (column & MID_RIGHT)
		return 0.25;
	return 0.75;
}

TileColumn get_column(Number x_pos)
{
	if (x_pos < -0.5)
		return FAR_LEFT;
	if (x_pos < 0)
		return MID_LEFT;
	if (x_pos < 0.5)
		return MID_RIGHT;
	return FAR_RIGHT;
}

TileColumn next_column(const std::shared_ptr<Tile>& previous_tile)
{
	if (!previous_tile)
	{
		switch (RNG::integer(0, 3))
		{
		case 0: return FAR_LEFT; break;
		case 1: return MID_LEFT; break;
		case 2: return MID_RIGHT; break;
		case 3: return FAR_RIGHT; break;
		}
	}
	else if (previous_tile->get_info().type == TileInfo::Type::SINGLE
		|| previous_tile->get_info().type == TileInfo::Type::LONG)
	{
		uint8_t random_column = RNG::integer(0, 2);
		TileColumn new_column;
		if (random_column == 0)
			new_column = FAR_LEFT;
		else if (random_column == 1)
			new_column = MID_LEFT;
		else new_column = MID_RIGHT;
		if (new_column == previous_tile->column)
			new_column = FAR_RIGHT;
		return new_column;
	}
	else if (previous_tile->get_info().type == TileInfo::Type::DOUBLE)
	{
		if (previous_tile->column == DT_LEFT)
		{
			return RNG::integer(0, 1) ? MID_LEFT : FAR_RIGHT;
		}
		return RNG::integer(0, 1) ? MID_RIGHT : FAR_LEFT;
	}
	else if (previous_tile->get_info().type == TileInfo::Type::EMPTY)
	{
		return next_column(reinterpret_cast<EmptyTile*>(previous_tile.get())->previous_tile);
	}
	else if (previous_tile->get_info().type == TileInfo::Type::SLIDER)
	{
		std::set<TileColumn> possible_columns = { FAR_LEFT, MID_LEFT, MID_RIGHT, FAR_RIGHT };
		auto end_columns = reinterpret_cast<SliderTile*>(previous_tile.get())->get_end_column();
		uint8_t mask = end_columns.first;
		if (end_columns.second.has_value())
			mask |= end_columns.second.value();
		for (uint8_t i = 0; i < possible_columns.size(); ++i)
		{
			if (mask & *std::next(possible_columns.begin(), i))
			{
				possible_columns.erase(std::next(possible_columns.begin(), i));
			}
		}
		return *std::next(possible_columns.begin(), RNG::integer(0, possible_columns.size() - 1));
	}
	abort();
}

bool Tile::is_active() const
{
	return id == (_level->cleared_tiles % _level->_song_info.tiles.size());
}

Number Tile::get_tile_length() const
{
	return Number(get_info().length) / Number(_level->_song_info.length_units_per_single_tile);
}

Number Tile::get_height() const
{
	return get_tile_length() / 4.0L;
}

const TileInfo& Tile::get_info() const
{
	return _level->_song_info.tiles[id];
}

void Tile::render_bg() const
{
}

void Tile::render_fg() const
{
}

void Tile::update()
{
	my_update();
	std::vector<std::pair<uint16_t,Vec2>> touch_down_sorted_positions;
	for (const auto& [finger_id, touch_pos] : _level->touch_down)
	{
		touch_down_sorted_positions.emplace_back(finger_id, touch_pos);
	}
	std::sort(touch_down_sorted_positions.begin(), touch_down_sorted_positions.end(),
		[](const std::pair<uint16_t, Vec2>& pos_a, const std::pair<uint16_t, Vec2>& pos_b) { return pos_a.second.y < pos_b.second.y; });

	for (const auto& [finger_id, touch_pos] : touch_down_sorted_positions)
	{
		touch_down(finger_id, { touch_pos.x, y_offset - (touch_pos.y + 1.0L) / 2.0L * 4.0L });
	}
	for (const auto& [finger_id, touch_pos] : _level->touch_move)
	{
		touch_move(finger_id, { touch_pos.x, y_offset - (touch_pos.y + 1.0L) / 2.0L * 4.0L });
	}
	for (const auto& [finger_id, touch_pos] : _level->touch_up)
	{
		touch_up(finger_id, { touch_pos.x, y_offset - (touch_pos.y + 1.0L) / 2.0L * 4.0L });
	}
}

void Tile::touch_down(uint16_t finger_id, Vec2 pos)
{
}

void Tile::touch_move(uint16_t finger_id, Vec2 pos)
{
}

void Tile::touch_up(uint16_t finger_id, Vec2 pos)
{
}

void Tile::my_update()
{
}

bool Tile::force_first_interaction() const
{
	return _level->force_first_interaction();
}

void Tile::handle_first_interaction()
{
	auto tile_index = static_cast<uint32_t>(_level->cleared_tiles % _level->_song_info.tiles.size());
	if (tile_index == 0)
	{
		_level->change_tempo(_level->_song_info.starting_tempo, _level->new_tp, _level->_position);
	}
	else if (_level->_song_info.tempo_changes.count(tile_index))
	{
		_level->change_tempo(_level->_song_info.tempo_changes.at(tile_index), _level->new_tp, _level->_position);
	}
}

void Tile::handle_clear()
{
	_level->cleared_tiles++;
}

void Tile::handle_game_over()
{
	_level->game_over(this);
}

void Tile::handle_state_change(const TileStateInfo* state)
{
	if (state->score_info.has_value())
	{
		auto points = state->score_info->points;
		state->score_info->silent ? _level->score.silent_add(points) : _level->score.add(points);
	}
}