#include "EmptyTile.h"

#include "../StateLevel.h"
#include "../Game.h"

#include <iostream>

EmptyTile::EmptyTile(StateLevel* level_)
	:StatableTile(level_, FAR_LEFT)
{
	if (get_info().length >= _level->_song_info.length_units_per_single_tile)
	{
		_level->previous_tile.reset();
	}
	else if (_level->previous_tile)
	{
		if (_level->previous_tile->get_info().type == TileInfo::Type::EMPTY)
		{
			std::cout << "WARNING: 2 consecutive empty tiles!" << std::endl;
			//prevent recursion
			previous_tile = reinterpret_cast<EmptyTile*>(_level->previous_tile.get())->previous_tile;
		}
		else
		{
			previous_tile = _level->previous_tile;
		}
	}
}

bool EmptyTile::should_game_over(Number y_offset) const
{
	return false;
}

bool EmptyTile::should_die(Number y_offset) const
{
	return true;
}

void EmptyTile::touch_down(uint16_t finger_id, Vec2 pos, bool force_first_interaction)
{
	if (is_state(&EmptyTileDefault))
	{
		if (pos.y >= 0 && pos.y < get_tile_length())
		{
			missed_column = get_column(pos.x);
			change_state(&EmptyTileMissed, force_first_interaction);
		}
	}
}

void EmptyTile::my_update(Number y_offset, bool force_first_interaction)
{
	if (is_state(&EmptyTileDefault))
	{
		change_state(&EmptyTileCleared, force_first_interaction);
	}
}

void EmptyTile::render_fg(Number y_offset) const
{
	if (is_state(&EmptyTileMissed) && (_level->new_tp % 0.25) > 0.125)
	{
		Vec2 pos = { get_column_x_pos(missed_column), y_offset / 4.0L * 2.0L - 1.0L };
		Texture* texture = &_level->txt_game_over_tile;
		_level->game->renderer->render(texture, {}, texture->get_psize(), pos,
			{ 0.25, get_height() }, {}, { 0,1 });
	}
}
