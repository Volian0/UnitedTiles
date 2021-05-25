#include "Tile.h"

#include "StateLevel.h"
#include "Game.h"
#include "AudioDevice.h"
#include "Renderer.h"
#include "RNG.h"

#include <iostream>

Tile::Tile(uint32_t tile_id_, StateLevel* level_, TileColumn column_)
	:tile_id{tile_id_},
	level{level_},
	info{ &level_->_song_info.tiles[tile_id_] },
	column{ column_ }
{
}

bool Tile::active() const
{
	return tile_id == level->cleared_tiles;
}

SingleTile::SingleTile(uint32_t tile_id_, StateLevel* level_)
	:Tile(tile_id_, level_, next_column(level_->previous_tile))
{
}

bool SingleTile::is_cleared() const
{
	return _cleared;
}

void SingleTile::render(Number y_offset) const
{
	Vec2 pos = { get_x_pos(column), y_offset / 4.0L * 2.0L - 1.0L };
	Texture* texture = _cleared ? &level->txt_single_tile_cleared : &level->txt_single_tile;
	level->game->renderer->render(texture, {}, texture->get_psize(), pos,
		{ 0.25, Number(info->length) / Number(level->_song_info.length_units_per_single_tile) / 4.0L }, {}, { 0,1 });
	if (game_over_column.has_value() && (level->new_tp.time_since_epoch() % std::chrono::milliseconds(250)) > std::chrono::milliseconds(125))
	{
		pos.x = get_x_pos(game_over_column.value());
		texture = &level->txt_game_over_tile;
		level->game->renderer->render(texture, {}, texture->get_psize(), pos,
			{ 0.25, Number(info->length) / Number(level->_song_info.length_units_per_single_tile) / 4.0L },{}, { 0,1 });
	}
}

TileColumn SingleTile::next_column(const std::shared_ptr<Tile>& previous_tile)
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
	else if (previous_tile->info->type == TileInfo::Type::SINGLE)
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
	else abort();
}

Number SingleTile::get_x_pos(TileColumn column_)
{
	if (column_ == FAR_LEFT)
		return -0.75L;
	else if (column_ == MID_LEFT)
		return -0.25L;
	else if (column_ == MID_RIGHT)
		return 0.25L;
	return 0.75L;
}

bool SingleTile::should_be_cleared(Number y_offset) const
{
	return y_offset > Number(info->length) / Number(level->_song_info.length_units_per_single_tile) + 4.0L;
}

bool SingleTile::should_die(Number y_offset) const
{
	return should_be_cleared(y_offset) && is_cleared();
}

bool SingleTile::touch_down(uint16_t finger_id, Vec2 pos)
{
	if (_cleared)
	{
		return true;
	}
	if (pos.y >= 0 && pos.y < Number(info->length) / Number(level->_song_info.length_units_per_single_tile))
	{
		TileColumn clicked_column;
		if (pos.x < -0.5)
			clicked_column = FAR_LEFT;
		else if (pos.x < 0)
			clicked_column = MID_LEFT;
		else if (pos.x < 0.5)
			clicked_column = MID_RIGHT;
		else clicked_column = FAR_RIGHT;
		if (clicked_column == column)
		{
			if (active())
			{
				_cleared = true;
				level->cleared_tiles++;
				level->queue_notes(info->note_events);
				level->score.add(1);
			}
		}
		else
		{
			game_over_column = clicked_column;
			return false;
		}
	}
	return true;
}
