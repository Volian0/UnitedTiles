#include "DoubleTile.h"

#include "../StateLevel.h"
#include "../Game.h"

#include "../RNG.h"
#include "EmptyTile.h"

DoubleTile::DoubleTile(StateLevel* level_)
	:StatableTile(level_, this->next_column(level_->previous_tile))
{
}

bool DoubleTile::should_game_over(Number y_offset) const
{
	return y_offset > get_tile_length() + 4.0L;
}

bool DoubleTile::should_die(Number y_offset) const
{
	return should_game_over(y_offset);
}

void DoubleTile::touch_down(uint16_t finger_id, Vec2 pos, bool force_first_interaction)
{
	if (pos.y >= 0 && pos.y < get_tile_length()
		&& (is_state(&DoubleTileDefault) || is_state(&DoubleTilePartiallyCleared)))
	{
		const auto clicked_column = get_column(pos.x);
		const auto missed = !static_cast<bool>(clicked_column & column);
		const auto clicked_left = !missed && (clicked_column & 0b11110000);
		missed_column = clicked_column;
		if (missed)
		{
			change_state(&DoubleTileMissed, force_first_interaction);
		}
		else if (is_state(&DoubleTileDefault))
		{
			left_tile_cleared = clicked_left;
			change_state(&DoubleTilePartiallyCleared, force_first_interaction);
		}
		else if (clicked_left ^ left_tile_cleared)
		{
			change_state(&DoubleTileFullyCleared, force_first_interaction);
		}
	}
}

void DoubleTile::on_changed_state()
{
	if (is_state(&DoubleTilePartiallyCleared) || is_state(&DoubleTileFullyCleared))
	{
		_level->_burst.add(Vec2(get_column_x_pos(missed_column), (_level->_position - _level->get_tile_pos(this) - get_tile_length() / 2.0L) / 4.0L * 2.0L - 1.0L), Vec2(0.25L, get_height()), get_tile_length() * 4.0L / _level->game->renderer->get_aspect_ratio(), 1.5L, Vec2(-1, -1), Vec2(1, 0), Colors::BLACK);
		if (is_state(&DoubleTilePartiallyCleared))
		{
			_level->queue_notes(get_info().note_events);
			tp_tapped = _level->new_tp;
		}
		else
		{
			Number half_tile_duration = get_tile_length() / 2.0L / _level->tps;
			if (_level->new_tp - *tp_tapped >= half_tile_duration)
			{
				_level->queue_notes(get_info().note_events_2nd_tile);
			}
			else
			{
				_level->queue_notes(get_info().note_events_2nd_tile, false, *tp_tapped + half_tile_duration);
			}
		}
	}
}

void DoubleTile::render_bg(Number y_offset) const
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

void DoubleTile::render_fg(Number y_offset) const
{
	if (!is_state(&DoubleTileFullyCleared))
	{
		Vec2 pos_left = { get_column_x_pos(column == DT_LEFT ? FAR_LEFT : MID_LEFT), y_offset / 4.0L * 2.0L - 1.0L };
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
	else abort();
}
