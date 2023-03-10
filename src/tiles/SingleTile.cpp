#include "SingleTile.h"

#include "../StateLevel.h"
#include "../Game.h"

SingleTile::SingleTile(StateLevel* level_)
	:StatableTile(level_, next_column(level_->previous_tile))
{
}

bool SingleTile::should_game_over() const
{
	return y_offset > get_tile_length() + 4.0L + _level->get_miss_range();
}
bool SingleTile::should_die() const
{
	return should_game_over();
}

void SingleTile::touch_down(uint16_t finger_id, Vec2 pos)
{
	if (is_state(&SingleTileDefault))
	{
		if (pos.y >= 0 && pos.y < get_tile_length())
		{
			TileColumn clicked_column = get_column(pos.x);
			if (clicked_column == column)
			{
				change_state(&SingleTileCleared);
			}
			else
			{
				missed_column = clicked_column;
				change_state(&SingleTileMissed);
			}
		}
	}
}

void SingleTile::on_changed_state()
{
	if (is_state(&SingleTileCleared))
	{
		_level->queue_notes(get_info().note_events);
		_level->_burst.add(Vec2(get_column_x_pos(column), (_level->_position - _level->get_tile_pos(this) - get_tile_length() / 2.0L) / 4.0L * 2.0L - 1.0L), Vec2(0.25L, get_height()), get_tile_length() * 3.0L / _level->game->renderer->get_aspect_ratio(), 1.5L, Vec2(-1, -1), Vec2(1, 0), Colors::BLACK);
	}
}

void SingleTile::render_fg() const
{
	Vec2 pos = { get_column_x_pos(column), y_offset / 4.0L * 2.0L - 1.0L };
	Texture* texture = is_state(&SingleTileCleared) ? &_level->txt_single_tile_cleared : &_level->txt_single_tile;
	_level->game->renderer->render(texture, {}, texture->get_psize(), pos,
		{ 0.25, get_height() }, {}, { 0,1 });
	if (is_state(&SingleTileMissed) && (_level->new_tp % 0.25) > 0.125)
	{
		pos.x = get_column_x_pos(missed_column);
		texture = &_level->txt_game_over_tile;
		_level->game->renderer->render(texture, {}, texture->get_psize(), pos,
			{ 0.25,  get_height() }, {}, { 0,1 });
	}
}