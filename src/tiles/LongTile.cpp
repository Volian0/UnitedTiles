#include "LongTile.h"

#include "../StateLevel.h"
#include "../Game.h"

#include <atomic>
#include <cmath>
#include <algorithm>
#include <iostream>

constexpr Number long_tile_helper = 0.125L + 0.03125L;

LongTile::LongTile(StateLevel* level_)
	:StatableTile(level_, next_column(level_->previous_tile))
{
}

bool LongTile::should_game_over() const
{
	return y_offset > 1.0L + 4.0L + _level->get_miss_range();
}

bool LongTile::should_die() const
{
	return y_offset > get_tile_length() + 4.0L + _level->get_miss_range();
}

void LongTile::my_update()
{
	if (is_state(&LongTileClearing))
	{
		held_tile_duration = y_offset - y_tapped + long_tile_helper;

		if (held_tile_duration + 0.21875L >= get_tile_length())
		{
			change_state(&LongTileFullyCleared);
		}
		else
		{
			const std::pair<Number, Number>* last_tap = nullptr;
			for (const auto& tap : taps)
			{
				if (tap.first <= held_tile_duration)
					last_tap = &tap;
				else break;
			}
			if (last_tap != nullptr)
			{
				tap_length = std::max(last_tap->second * (1.0L - (held_tile_duration - last_tap->first)), 0.0L);
			}
		}
	}
		//else tap_length = 0.0L;
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
					//y_finger_tapped = pos.y;
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

Number LongTile::y_finger_tapped = 0.0L;

void LongTile::on_changed_state()
{
	if (is_state(&LongTileClearing))
	{
		_level->queue_notes(get_info().note_events);
		//tp_tapped = _level->new_tp;
		y_tapped = y_offset - y_finger_tapped;

		//add taps here
		{
			const auto& note_events = get_info().note_events;
			for (const auto& it : note_events)
			{
				const auto& event = it.second;
				if (event.type != NoteEvent::Type::ON)
					continue;
				const auto length = Number(it.first) / _level->_song_info.note_ticks_per_single_tile + y_finger_tapped + long_tile_helper;
				const auto strength = Number(event.velocity) / 127.0L;
				if (!taps.empty())
				{
					auto& back = taps.back();
					if (back.first >= length)
					{
						back.second = std::max(back.second, strength);
						continue;
					}
				}
				taps.emplace_back(length, strength);
			}
		}

		my_update();
	}
	else if (is_state(&LongTileFullyCleared))
	{
		_level->_burst.add(Vec2(get_column_x_pos(column), (_level->_position - _level->get_tile_pos(this) - get_tile_length() / 2.0L) / 4.0L * 2.0L - 1.0L), Vec2(0.25L, get_height()), get_tile_length() * 4.0L / _level->game->renderer->get_aspect_ratio(), 1.5L, Vec2(-1, -1), Vec2(1, 0), Colors::WHITE);
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
				{ 0.25, (held_tile_duration) / 4.0L }, {}, { 0,1 });
			texture = &_level->txt_long_tile_end;
			_level->game->renderer->render(texture, {}, texture->get_psize(), pos - Vec2{ 0, (held_tile_duration) / 2.0L },
				{ 0.25, 0.25L / 4.0L }, {}, { 0,1 });
			texture = &_level->txt_long_tile_circle;
			if (is_state(&LongTileClearing))
			{
				Color previous_tint = texture->tint;



				texture->tint = {(tap_length*127.0L + 128.0L) * (Number(previous_tint.r) / 255.0), (tap_length*127.0L + 128.0L) * (Number(previous_tint.g) / 255.0), previous_tint.b, tap_length*200.0L};
				_level->game->renderer->render(texture, {}, texture->get_psize(), pos - Vec2{ 0, (held_tile_duration) / 2.0L+0.100L },
					Vec2{ 0.25L, 0.25L * _level->game->renderer->get_aspect_ratio() }*(tap_length+0.0625L), {});
				texture->tint = previous_tint;
			}
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