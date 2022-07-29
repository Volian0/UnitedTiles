#include "SliderTile.h"

#include "../StateLevel.h"
#include "../Game.h"

#include "../RNG.h"
#include "EmptyTile.h"

#include <algorithm>

SliderTile::SliderTile(StateLevel* level_)
	:StatableTile(level_, this->next_column(level_->previous_tile)),
	_column_index{get_column_index()}
{
}

bool SliderTile::should_game_over() const
{
	return y_offset > 1.0L + 4.0L;
}

bool SliderTile::should_die() const
{
	return y_offset > get_tile_length() + 4.0L;
}

void SliderTile::my_update()
{
	//very hacky, but it works
	finger_pos.y += y_offset - previous_offset.value_or(y_offset);
	previous_offset = y_offset;
	//
	if (is_state(&SliderTileClearing))
	{
		if (in_range(finger_pos))
		{
			held_tile_duration = std::max(finger_pos.y, held_tile_duration);
			if (held_tile_duration + 1.0L >= get_tile_length())
			{
				change_state(&SliderTileCleared);
			}
		}
		else
		{
			change_state(&SliderTileMissed);
		}
	}
}

void SliderTile::touch_down(uint16_t finger_id, Vec2 pos)
{
	if (is_state(&SliderTileDefault))
	{
		if (pos.y >= 0 && pos.y < 1.0L && in_range(pos))
		{
			finger = finger_id;
			finger_pos = pos;
			change_state(&SliderTileClearing);
		}
	}
}

void SliderTile::touch_move(uint16_t finger_id, Vec2 pos)
{
	if (is_state(&SliderTileClearing) && finger_id == finger)
	{
		finger_pos = pos;
	}
}

void SliderTile::touch_up(uint16_t finger_id, Vec2 pos)
{
	if (is_state(&SliderTileClearing) && finger_id == finger)
	{
		change_state(&SliderTileMissed);
	}
}

void SliderTile::on_changed_state()
{
	if (is_state(&SliderTileClearing))
	{
		_level->queue_notes(get_info().note_events);
		my_update();
	}
}

void SliderTile::render_fg() const
{
	if (is_state(&SliderTileMissed) && (_level->new_tp % 0.25) > 0.125)
	{
		return;
	}
	const auto length = get_info().length;
	const uint32_t length_per_single_tile = _level->_song_info.length_units_per_single_tile;
	const auto iterations = (length * 2 - 1) / length_per_single_tile + 1;
	auto last_iteration_length = length % (length_per_single_tile / 2);
	if (last_iteration_length == 0)
	{
		last_iteration_length = length_per_single_tile / 2;
	}
	uint8_t current_column_index = _column_index;
	Number total_held_left = held_tile_duration;
	for (uint32_t i = 0; i < iterations; ++i)
	{
		Number y_length = (i == iterations - 1) ? Number(last_iteration_length) / Number(length_per_single_tile) : 0.5L;
		bool facing_right = current_column_index >= 3;

		Number x_pos = get_column_x_pos(columns[current_column_index]);

		auto texture = &_level->slider_tile;
		auto texture2 = &_level->slider_tile_clearing;

		if (is_state(&SliderTileCleared))
		{
			texture = texture2;
		}

		Vec2 pos = { x_pos, (y_offset - Number(i) * 0.5L) / 4.0L * 2.0L - 1.0L };

		render_fragment(texture, y_length, pos, facing_right);

		if ((is_state(&SliderTileClearing) || is_state(&SliderTileMissed)) && total_held_left > 0)
		{
			render_fragment(texture2, std::min(y_length,total_held_left), pos, facing_right);
		}

		current_column_index = (current_column_index + 1) % columns.size();

		total_held_left -= y_length;
	}
}

TileColumn SliderTile::next_column(const std::shared_ptr<Tile>& previous_tile) const
{
	switch (::next_column(previous_tile))
	{
	case FAR_LEFT:  return ST_FAR_LEFT;  break;
	case MID_LEFT:  return RNG::boolean() ? ST_MID_LEFT_FACING_LEFT  : ST_MID_LEFT_FACING_RIGHT;  break;
	case MID_RIGHT: return RNG::boolean() ? ST_MID_RIGHT_FACING_LEFT : ST_MID_RIGHT_FACING_RIGHT; break;
	case FAR_RIGHT: return ST_FAR_RIGHT; break;
	}
	abort();
}

bool SliderTile::in_range(Vec2 pos) const
{
	return std::abs((std::abs(std::fmod(pos.y + Number(_column_index) * 0.5L, 3.0L) - 1.5L) - 0.75L) - pos.x) < 0.75L;
}

uint8_t SliderTile::get_column_index() const
{
	for (uint8_t index = 0; index < columns.size(); ++index)
	{
		if (columns[index] == column)
		{
			return index;
		}
	}
	abort();
}

void SliderTile::render_fragment(Texture* texture, Number length, Vec2 pos, bool facing_right) const
{
	length = std::clamp(length, 0.0L, 1.0L);
	auto psize = texture->get_psize();
	auto source_size = glm::u32vec2(Vec2(psize) * length);
	source_size.y = std::clamp(source_size.y, 1U, psize.y);
	_level->game->renderer->render(texture, { 0, psize.y - source_size.y }, { psize.x, source_size.y }, pos,
		{ 1.0, length / 4.0L }, {}, { facing_right ? -0.5 : 0.5,1 }, 0.0L, !facing_right);
}

std::pair<TileColumn, std::optional<TileColumn>> SliderTile::get_end_column() const
{
	const auto length = get_info().length;
	const uint32_t length_per_single_tile = _level->_song_info.length_units_per_single_tile;
	const auto iterations = (length * 2 - 1) / length_per_single_tile + 1;
	auto last_iteration_length = length % (length_per_single_tile / 2);
	uint8_t current_column_index = _column_index;
	uint8_t last_column_index;
	for (uint32_t i = 0; i < iterations; ++i)
	{
		last_column_index = current_column_index;
		current_column_index = (current_column_index + 1) % columns.size();
	}
	std::optional<TileColumn> second_column;
	return { columns[current_column_index], last_iteration_length ? std::make_optional<TileColumn>(columns[last_column_index]) : std::nullopt };
}
