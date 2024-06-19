#include "DoubleTile.h"

#include "../StateLevel.h"
#include "../Game.h"

#include "../RNG.h"
#include "EmptyTile.h"
#include "SliderTile.h"
#include "SingleTile.h"

DoubleTile::DoubleTile(StateLevel* level_)
	:StatableTile(level_, this->next_column(level_->previous_tile))
{
}

bool DoubleTile::should_game_over() const
{
	return y_offset > get_tile_length() + 4.0L + _level->get_miss_range();
}

bool DoubleTile::should_die() const
{
	return should_game_over();
}

void DoubleTile::revive()
{
	if (is_state(&DoubleTileMissed) || is_state(&DoubleTilePartiallyCleared))
	{
		force_change_state(&DoubleTileFullyCleared);
		_level->cleared_tiles++;
	}
}

bool DoubleTile::touch_down(uint16_t finger_id, Vec2 pos)
{
	const auto helper = is_active() ? _level->get_miss_range() : 0.0L;
	if (pos.y >= -helper && pos.y <= get_tile_length() + helper
		&& (is_state(&DoubleTileDefault) || is_state(&DoubleTilePartiallyCleared)))
	{
		const auto clicked_column = get_column(pos.x);
		const auto missed = !static_cast<bool>(clicked_column & column);
		const auto clicked_left = !missed && (clicked_column & 0b11110000);
		missed_column = clicked_column;
		if (missed)
		{
			return change_state(&DoubleTileMissed);
		}
		else if (is_state(&DoubleTileDefault))
		{
			left_tile_cleared = clicked_left;
			m_cleared_positions[left_tile_cleared ? 0 : 1] = y_offset;
			return change_state(&DoubleTilePartiallyCleared);
		}
		else if (clicked_left ^ left_tile_cleared)
		{
			m_cleared_positions[left_tile_cleared ? 1 : 0] = y_offset;
			return change_state(&DoubleTileFullyCleared);
		}
	}
	return false;
}

void DoubleTile::on_changed_state()
{
	if (is_state(&DoubleTilePartiallyCleared) || is_state(&DoubleTileFullyCleared))
	{
		_level->_burst.add(Vec2(get_column_x_pos(missed_column), (_level->_position - _level->get_tile_pos(this) - get_tile_length() / 2.0L) / 4.0L * 2.0L - 1.0L), Vec2(0.25L, get_height()), get_tile_length() * 2.0L / _level->game->renderer->get_aspect_ratio(), 1.5L, Vec2(-1, -1), Vec2(1, 0), Colors::BLACK);
		if (is_state(&DoubleTilePartiallyCleared))
		{
			_level->queue_notes(get_info().note_events);
			tp_tapped = _level->new_tp;
			if (_level->_song_info.acceleration_method == SongInfo::AccelerationMethod::ACCELERATION)
			{
				const auto parameter = _level->_song_info.acceleration_info.parameter;
				const auto start_tps = _level->tps;
				const auto& timepoint = _level->new_tp;
				const auto desired_length = get_tile_length() / 2.0L;
				const auto offset_s = (std::sqrt(start_tps * start_tps + 2.0L * parameter * desired_length) - start_tps) / parameter;
				tp_second = *tp_tapped + offset_s;
			}
			else
			{
				tp_second = *tp_tapped + get_tile_length() / 2.0L / _level->tps;
			}
		}
		else
		{
			if (_level->new_tp >= *tp_second)
			{
				_level->queue_notes(get_info().note_events_2nd_tile);
			}
			else
			{
				_level->queue_notes(get_info().note_events_2nd_tile, false, tp_second);
			}
		}
	}
}

void DoubleTile::render_bg() const
{
	const Vec2 pos_left = { get_column_x_pos(column == DT_LEFT ? FAR_LEFT : MID_LEFT), y_offset / 4.0L * 2.0L - 1.0L };
	if (is_state(&DoubleTilePartiallyCleared) || is_state(&DoubleTileFullyCleared) || (is_state(&DoubleTileMissed) && tp_tapped.has_value()))
	{
		for (uint8_t i = 0; i < 2; ++i)
		{
			if (is_state(&DoubleTileFullyCleared) || ((i == 1) ^ left_tile_cleared))
			{
				_level->game->renderer->render(&_level->txt_single_tile_cleared, {}, _level->txt_single_tile_cleared.get_psize(),
					{ pos_left.x + Number(i) * 1.0L, pos_left.y }, { 0.25, get_height() }, {}, { 0,1 });
			}
		}
	}
}

void DoubleTile::render_fg() const
{
	Vec2 pos_left = { get_column_x_pos(column == DT_LEFT ? FAR_LEFT : MID_LEFT), y_offset / 4.0L * 2.0L - 1.0L };
	if ((is_state(&DoubleTileFullyCleared) || is_state(&DoubleTilePartiallyCleared))
		&& !_level->is_game_over())
	{
		for (uint8_t i = 0; i < 2; ++i)
		{
			if ((!is_state(&DoubleTileFullyCleared)) && ((i == 0) ^ left_tile_cleared))
				continue;

			const auto offset = y_offset - m_cleared_positions[i];
			if (offset < 2.0L / 3.0L)
			{
				SingleTile::render_clearing(offset, &_level->txt_single_tile, Vec2{pos_left.x + Number(i), pos_left.y}, get_height(), _level->game->renderer.get());
			}
		}
	}
	if (!is_state(&DoubleTileFullyCleared))
	{
		for (uint8_t i = 0; i < 2; ++i)
		{
			if (!tp_tapped.has_value() || ((i == 0) ^ left_tile_cleared))
			{
				_level->game->renderer->render(&_level->txt_single_tile, {}, _level->txt_single_tile.get_psize(),
					{ pos_left.x + Number(i) * 1.0L, pos_left.y }, { 0.25, get_height() }, {}, { 0,1 });
			}
		}
		if (is_state(&DoubleTileMissed) && (_level->new_tp % 0.25) > 0.125)
		{
			pos_left.x = get_column_x_pos(missed_column);
			auto texture = &_level->txt_game_over_tile;
			_level->game->renderer->render(texture, {}, texture->get_psize(), pos_left,
				{ 0.25,  get_height() }, {}, { 0,1 });
		}
	}
}

TileColumn DoubleTile::next_column(const std::shared_ptr<Tile>& previous_tile)
{
	if (!previous_tile)
	{
		return RNG::integer(0, 1) ? DT_LEFT : DT_RIGHT;
	}
	else if (previous_tile->get_info().type == TileInfo::Type::SINGLE
		|| previous_tile->get_info().type == TileInfo::Type::LONG)
	{
		return (previous_tile->column == FAR_LEFT || previous_tile->column == MID_RIGHT) ? DT_RIGHT : DT_LEFT;
	}
	else if (previous_tile->get_info().type == TileInfo::Type::DOUBLE)
	{
		return previous_tile->column == DT_LEFT ? DT_RIGHT : DT_LEFT;
	}
	else if (previous_tile->get_info().type == TileInfo::Type::EMPTY)
	{
		return this->next_column(reinterpret_cast<EmptyTile*>(previous_tile.get())->previous_tile);
	}
	else if (previous_tile->get_info().type == TileInfo::Type::SLIDER)
	{
		auto end_columns = reinterpret_cast<SliderTile*>(previous_tile.get())->get_end_column();
		if (end_columns.first & 0b11001100)
		{
			return DT_RIGHT;
		}
		return DT_LEFT;
	}
	else abort();
}
