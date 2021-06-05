#include "Tile.h"

#include "StateLevel.h"
#include "Game.h"
#include "AudioDevice.h"
#include "Renderer.h"
#include "RNG.h"
#include "Color.h"

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
	return tile_id == (level->cleared_tiles % level->_song_info.tiles.size());
}

bool Tile::touch_down(uint16_t finger_id, Vec2 pos)
{
	return true;
}

bool Tile::touch_up(uint16_t finger_id, Vec2 pos)
{
	return true;
}

bool Tile::touch_move(uint16_t finger_id, Vec2 pos)
{
	return true;
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
	if (game_over_column.has_value() && (level->new_tp % 0.25) > 0.125)
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
		default: abort(); break;
		}
	}
	else if (previous_tile->info->type == TileInfo::Type::SINGLE
		||   previous_tile->info->type == TileInfo::Type::LONG)
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
	else if (previous_tile->info->type == TileInfo::Type::DOUBLE)
	{
		if (previous_tile->column == DT_LEFT)
		{
			return RNG::integer(0, 1) ? MID_LEFT : FAR_RIGHT;
		}
		return RNG::integer(0, 1) ? MID_RIGHT : FAR_LEFT;
	}
	else if (previous_tile->info->type == TileInfo::Type::EMPTY)
	{
		return next_column(reinterpret_cast<EmptyTile*>(previous_tile.get())->previous_tile);
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
	Number tile_length = Number(info->length) / Number(level->_song_info.length_units_per_single_tile);
	if (pos.y >= 0 && pos.y < tile_length)
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
				level->_burst.add(Vec2(get_x_pos(column), (level->_position - level->get_tile_pos(this) - tile_length/2.0L) / 4.0L * 2.0L - 1.0L), Vec2(0.25L, tile_length / 4.0L), tile_length * 5.0L, 1.5L, Vec2(-1, -1), Vec2(1, 0), Color::BLACK);
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

LongTile::LongTile(uint32_t tile_id_, StateLevel* level_)
	:Tile(tile_id_, level_, SingleTile::next_column(level_->previous_tile))
{
}

bool LongTile::is_cleared() const
{
	return finger_id_clicked.has_value();
}

void LongTile::render(Number y_offset) const
{
	Vec2 pos = { SingleTile::get_x_pos(column), y_offset / 4.0L * 2.0L - 1.0L };
	Texture* texture;
	if (fully_cleared)
	{
		texture = &level->txt_single_tile_cleared;
		level->game->renderer->render(texture, {}, texture->get_psize(), pos,
			{ 0.25, Number(info->length) / Number(level->_song_info.length_units_per_single_tile) / 4.0L }, {}, { 0,1 });
	}
	else
	{
		texture = &level->txt_long_tile;
		level->game->renderer->render(texture, {}, texture->get_psize(), pos,
			{ 0.25, 0.25L }, {}, { 0,1 });
		texture = &level->txt_long_tile_ext;
		level->game->renderer->render(texture, {}, texture->get_psize(), pos + Vec2{0, -0.5},
			{ 0.25, Number(info->length) / Number(level->_song_info.length_units_per_single_tile) / 4.0L - 0.25L }, {}, { 0,1 });
		texture = &level->tile_divider;
		Number div_offset = -0.25L - 0.25L * level->game->renderer->get_aspect_ratio() * 0.33203125L;
		level->game->renderer->render(texture, {}, texture->get_psize(), pos + Vec2{ 0, div_offset },
			{ 0.01L, Number(info->length) / Number(level->_song_info.length_units_per_single_tile) / 4.0L + div_offset/2.0L - 0.0625L }, {}, { 0,1 });
		if (finger_id_clicked.has_value())
		{
			texture = &level->txt_long_tile_clearing;
			level->game->renderer->render(texture, {}, texture->get_psize(), pos,
				{ 0.25, (held_tile_duration + 0.5L) / 4.0L }, {}, { 0,1 });
			texture = &level->txt_long_tile_end;
			level->game->renderer->render(texture, {}, texture->get_psize(), pos - Vec2{0, (held_tile_duration + 0.5L) / 2.0L },
				{ 0.25, 0.5L / 4.0L }, {}, { 0,1 });
		}
		else
		{
			texture = &level->txt_long_tile_circle;
			level->game->renderer->render(texture, {}, texture->get_psize(), pos + Vec2{0, -0.25},
				{ 0.125, 0.125L*level->game->renderer->get_aspect_ratio() }, {}, { 0,0 });
		}
	}
	if (game_over_column.has_value() && (level->new_tp % 0.25) > 0.125)
	{
		pos.x = SingleTile::get_x_pos(game_over_column.value());
		texture = &level->txt_game_over_tile;
		level->game->renderer->render(texture, {}, texture->get_psize(), pos,
			{ 0.25, Number(info->length) / Number(level->_song_info.length_units_per_single_tile) / 4.0L }, {}, { 0,1 });
	}
}

bool LongTile::should_be_cleared(Number y_offset) const
{
	if (_is_held)
	{
		held_tile_duration = (level->new_tp - finger_id_clicked.value().second) * level->tps;
		Number tile_length = Number(info->length) / Number(level->_song_info.length_units_per_single_tile);
		if (held_tile_duration + 1.0L >= tile_length)
		{
			_is_held = false;
			fully_cleared = true;
			level->score.add(1);
			level->_burst.add(Vec2(SingleTile::get_x_pos(column), (level->_position - level->get_tile_pos(this) - tile_length / 2.0L) / 4.0L * 2.0L - 1.0L), Vec2(0.25L, tile_length / 4.0L), tile_length * 5.0L, 1.5L, Vec2(-1, -1), Vec2(1, 0), Color::CYAN);
		}
	}
	return y_offset > 1.0L + 4.0L;
}

bool LongTile::should_die(Number y_offset) const
{
	return is_cleared() && y_offset > Number(info->length) / Number(level->_song_info.length_units_per_single_tile) + 4.0L;
}

bool LongTile::touch_down(uint16_t finger_id, Vec2 pos)
{
	if (is_cleared())
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
			if (active() && pos.y < 1.0L)
			{
				finger_id_clicked = {finger_id, level->new_tp};
				_is_held = true;
				level->cleared_tiles++;
				level->queue_notes(info->note_events);
				level->score.silent_add(1);
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

bool LongTile::touch_up(uint16_t finger_id, Vec2 pos)
{
	if (_is_held && finger_id == finger_id_clicked.value().first)
	{
		_is_held = false;
		level->score.silent_update();
	}
	return true;
}

DoubleTile::DoubleTile(uint32_t tile_id_, StateLevel* level_)
	:Tile(tile_id_, level_, next_column(level_->previous_tile))
{
	
}

bool DoubleTile::is_cleared() const
{
	return _cleared_left && _cleared_right;
}

void DoubleTile::render(Number y_offset) const
{
	Vec2 pos_left = { SingleTile::get_x_pos(column == DT_LEFT ? FAR_LEFT : MID_LEFT), y_offset / 4.0L * 2.0L - 1.0L };
	Vec2 pos_right = { SingleTile::get_x_pos(column == DT_LEFT ? MID_RIGHT : FAR_RIGHT), y_offset / 4.0L * 2.0L - 1.0L };
	Texture* texture_left = _cleared_left ? &level->txt_single_tile_cleared : &level->txt_single_tile;
	Texture* texture_right = _cleared_right ? &level->txt_single_tile_cleared : &level->txt_single_tile;
	level->game->renderer->render(texture_left, {}, texture_left->get_psize(), pos_left,
		{ 0.25, Number(info->length) / Number(level->_song_info.length_units_per_single_tile) / 4.0L }, {}, { 0,1 });
	level->game->renderer->render(texture_right, {}, texture_right->get_psize(), pos_right,
		{ 0.25, Number(info->length) / Number(level->_song_info.length_units_per_single_tile) / 4.0L }, {}, { 0,1 });
	if (game_over_column.has_value() && (level->new_tp % 0.25) > 0.125)
	{
		pos_left.x = SingleTile::get_x_pos(game_over_column.value());
		texture_left = &level->txt_game_over_tile;
		level->game->renderer->render(texture_left, {}, texture_left->get_psize(), pos_left,
			{ 0.25, Number(info->length) / Number(level->_song_info.length_units_per_single_tile) / 4.0L }, {}, { 0,1 });
	}
}

bool DoubleTile::should_be_cleared(Number y_offset) const
{
	return y_offset > Number(info->length) / Number(level->_song_info.length_units_per_single_tile) + 4.0L;
}

bool DoubleTile::should_die(Number y_offset) const
{
	return should_be_cleared(y_offset) && is_cleared();
}

bool DoubleTile::touch_down(uint16_t finger_id, Vec2 pos)
{
	if (is_cleared())
	{
		return true;
	}
	Number tile_length = Number(info->length) / Number(level->_song_info.length_units_per_single_tile);
	if (pos.y >= 0 && pos.y < tile_length)
	{
		TileColumn clicked_column;
		if (pos.x < -0.5)
			clicked_column = FAR_LEFT;
		else if (pos.x < 0)
			clicked_column = MID_LEFT;
		else if (pos.x < 0.5)
			clicked_column = MID_RIGHT;
		else clicked_column = FAR_RIGHT;
		if (clicked_column & column)
		{
			if (!active())
				return true;

			//handle already clicked tiles
			if (clicked_column & 0b11110000)
			{
				if (_cleared_left)
					return true;
				else
					_cleared_left = true;
			}
			else if (_cleared_right)
				return true;
			else
				_cleared_right = true;

			//handle note events
			if (!clicked.has_value())
			{
				level->queue_notes(info->note_events);
				level->score.add(1);
				clicked = level->new_tp;
				level->_burst.add(Vec2(SingleTile::get_x_pos(clicked_column), (level->_position - level->get_tile_pos(this) - tile_length / 2.0L) / 4.0L * 2.0L - 1.0L), Vec2(0.25L, tile_length / 4.0L), tile_length * 5.0L, 1.5L, Vec2(-1, -1), Vec2(1, 0), Color::BLACK);
			}
			else
			{
				Number half_tile_duration = 0.5L / level->tps;
				if (level->new_tp - clicked.value() >= half_tile_duration)
				{
					level->queue_notes(info->note_events_2nd_tile);
				}
				else
				{
					level->queue_notes(info->note_events_2nd_tile, false, clicked.value() + half_tile_duration);
				}
				level->score.add(1);
				level->cleared_tiles++;
				level->_burst.add(Vec2(SingleTile::get_x_pos(clicked_column), (level->_position - level->get_tile_pos(this) - tile_length / 2.0L) / 4.0L * 2.0L - 1.0L), Vec2(0.25L, tile_length / 4.0L), tile_length * 5.0L, 1.5L, Vec2(-1, -1), Vec2(1, 0), Color::BLACK);
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

TileColumn DoubleTile::next_column(const std::shared_ptr<Tile>& previous_tile)
{
	if (!previous_tile)
	{
		return RNG::integer(0, 1) ? DT_LEFT : DT_RIGHT;
	}
	else if (previous_tile->info->type == TileInfo::Type::SINGLE
		|| previous_tile->info->type == TileInfo::Type::LONG)
	{
		return (previous_tile->column == FAR_LEFT || previous_tile->column == MID_RIGHT) ? DT_RIGHT : DT_LEFT;
	}
	else if (previous_tile->info->type == TileInfo::Type::DOUBLE)
	{
		return previous_tile->column == DT_LEFT ? DT_RIGHT : DT_LEFT;
	}
	else if (previous_tile->info->type == TileInfo::Type::EMPTY)
	{
		return next_column(reinterpret_cast<EmptyTile*>(previous_tile.get())->previous_tile);
	}
	else abort();
}

EmptyTile::EmptyTile(uint32_t tile_id_, StateLevel* level_)
	:Tile(tile_id_, level_, FAR_LEFT)
{
	if (info->length >= level->_song_info.length_units_per_single_tile)
	{
		level->previous_tile.reset();
	}
	else if (level->previous_tile)
	{
		if (level->previous_tile->info->type == TileInfo::Type::EMPTY)
		{
			std::cout << "WARNING: 2 consecutive empty tiles!" << std::endl;

			//prevent recursion
			previous_tile = reinterpret_cast<EmptyTile*>(level->previous_tile.get())->previous_tile;
		}
		else
		{
			previous_tile = level->previous_tile;
		}
	}
}

bool EmptyTile::is_cleared() const
{
	return _cleared;
}

void EmptyTile::render(Number y_offset) const
{
	Vec2 pos = { 0, y_offset / 4.0L * 2.0L - 1.0L };
	if (game_over_column.has_value() && (level->new_tp % 0.25) > 0.125)
	{
		pos.x = SingleTile::get_x_pos(game_over_column.value());
		Texture* texture = &level->txt_game_over_tile;
		level->game->renderer->render(texture, {}, texture->get_psize(), pos,
			{ 0.25, Number(info->length) / Number(level->_song_info.length_units_per_single_tile) / 4.0L }, {}, { 0,1 });
	}
}

bool EmptyTile::should_be_cleared(Number y_offset) const
{
	if (active())
	{
		_cleared = true;
		level->cleared_tiles++;
	}
	return _cleared;
}

bool EmptyTile::should_die(Number y_offset) const
{
	return _cleared;
}

bool EmptyTile::touch_down(uint16_t finger_id, Vec2 pos)
{
	if (_cleared)
	{
		return true;
	}
	else if (active())
	{
		level->cleared_tiles++;
		_cleared = true;
	}
	else if (pos.y >= 0 && pos.y < Number(info->length) / Number(level->_song_info.length_units_per_single_tile))
	{
		TileColumn clicked_column;
		if (pos.x < -0.5)
			clicked_column = FAR_LEFT;
		else if (pos.x < 0)
			clicked_column = MID_LEFT;
		else if (pos.x < 0.5)
			clicked_column = MID_RIGHT;
		else clicked_column = FAR_RIGHT;
			game_over_column = clicked_column;
			return false;
	}
	return true;
}
