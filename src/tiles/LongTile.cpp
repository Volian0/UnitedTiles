#include "LongTile.h"

#include "../StateLevel.h"
#include "../Game.h"

LongTile::LongTile(StateLevel* level_)
	:StatableTile(level_, next_column(level_->previous_tile))
{
}

bool LongTile::should_game_over() const
{
	return y_offset > 1.0L + 4.0L;
}

bool LongTile::should_die() const
{
	return y_offset > get_tile_length() + 4.0L;;
}

void LongTile::my_update()
{
	if (is_state(&LongTileClearing))
	{
		held_tile_duration = (_level->new_tp - tp_tapped) * _level->tps;

		if (held_tile_duration + 1.0L >= get_tile_length())
		{
			change_state(&LongTileFullyCleared);
		}
	}
}

void LongTile::touch_down(uint16_t finger_id, Vec2 pos)
{
	if (is_state(&LongTileDefault))
	{
		if (pos.y >= 0 && pos.y < get_tile_length())
		{
			auto clicked_column = get_column(pos.x);
			if (clicked_column == column)
			{
				if (pos.y < 1.0L)
				{
					finger = finger_id;
					change_state(&LongTileClearing);
				}
			}
			else
			{
				missed_column = clicked_column;
				change_state(&LongTileMissed);
			}
		}
	}
}

void LongTile::touch_up(uint16_t finger_id, Vec2 pos)
{
	if (is_state(&LongTileClearing) && finger_id == finger)
	{
		change_state(&LongTileNotFullyCleared);
	}
}

void LongTile::on_changed_state()
{
	if (is_state(&LongTileClearing))
	{
		_level->queue_notes(get_info().note_events);
		tp_tapped = _level->new_tp;
	}
	else if (is_state(&LongTileFullyCleared))
	{
		_level->_burst.add(Vec2(get_column_x_pos(column), (_level->_position - _level->get_tile_pos(this) - get_tile_length() / 2.0L) / 4.0L * 2.0L - 1.0L), Vec2(0.25L, get_height()), get_tile_length() * 4.0L / _level->game->renderer->get_aspect_ratio(), 1.5L, Vec2(-1, -1), Vec2(1, 0), Colors::CYAN);
	}
}

void LongTile::render_fg() const
{
	Vec2 pos = { get_column_x_pos(column), y_offset / 4.0L * 2.0L - 1.0L };
	Texture* texture;
	if (is_state(&LongTileFullyCleared))
	{
		texture = &_level->txt_single_tile_cleared;
		_level->game->renderer->render(texture, {}, texture->get_psize(), pos,
			{ 0.25, get_height() }, {}, { 0,1 });
	}
	else
	{
		texture = &_level->txt_long_tile;
		_level->game->renderer->render(texture, {}, texture->get_psize(), pos,
			{ 0.25, 0.25L }, {}, { 0,1 });
		texture = &_level->txt_long_tile_ext;
		_level->game->renderer->render(texture, {}, texture->get_psize(), pos + Vec2{ 0, -0.5 },
			{ 0.25, get_height() - 0.25L }, {}, { 0,1 });
		texture = &_level->tile_divider;
		Number div_offset = -0.25L - 0.25L * _level->game->renderer->get_aspect_ratio() * 0.33203125L;
		_level->game->renderer->render(texture, {}, texture->get_psize(), pos + Vec2{ 0, div_offset },
			{ 0.01L, get_height() + div_offset / 2.0L - 0.0625L }, {}, { 0,1 });
		if (is_state(&LongTileClearing) || is_state(&LongTileNotFullyCleared))
		{
			texture = &_level->txt_long_tile_clearing;
			_level->game->renderer->render(texture, {}, texture->get_psize(), pos,
				{ 0.25, (held_tile_duration + 0.5L) / 4.0L }, {}, { 0,1 });
			texture = &_level->txt_long_tile_end;
			_level->game->renderer->render(texture, {}, texture->get_psize(), pos - Vec2{ 0, (held_tile_duration + 0.5L) / 2.0L },
				{ 0.25, 0.5L / 4.0L }, {}, { 0,1 });
		}
		else
		{
			texture = &_level->txt_long_tile_circle;
			_level->game->renderer->render(texture, {}, texture->get_psize(), pos + Vec2{ 0, -0.25 },
				{ 0.125, 0.125L * _level->game->renderer->get_aspect_ratio() }, {}, { 0,0 });
		}
	}
	if (is_state(&LongTileMissed) && (_level->new_tp % 0.25) > 0.125)
	{
		pos.x = get_column_x_pos(missed_column);
		texture = &_level->txt_game_over_tile;
		_level->game->renderer->render(texture, {}, texture->get_psize(), pos,
			{ 0.25, get_height() }, {}, { 0,1 });
	}
}