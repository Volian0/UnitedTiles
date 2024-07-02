#include "SingleTile.h"

#include "../StateLevel.h"
#include "../Game.h"
#include "../RNG.h"

#include <algorithm>

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

void SingleTile::revive()
{
	if (is_state(&SingleTileMissed))
	{
		force_change_state(&SingleTileDefault);
	}
}

bool SingleTile::touch_down(uint16_t finger_id, Vec2 pos)
{
	if (is_state(&SingleTileDefault))
	{
		const auto helper = is_active() ? _level->get_miss_range() : 0.0L;
		if (pos.y >= -helper && pos.y <= get_tile_length() + helper)
		{
			TileColumn clicked_column = get_column(pos.x);
			if (clicked_column == column)
			{
				return change_state(&SingleTileCleared);
			}
			else
			{
				random_jken = RNG::integer(1, 10000);
				missed_column = clicked_column;
				return change_state(&SingleTileMissed);
			}
		}
	}
	return false;
}

void SingleTile::on_changed_state()
{
	if (is_state(&SingleTileCleared))
	{
		m_cleared_position = y_offset;
		_level->queue_notes(get_info().note_events);
		_level->_burst.add(Vec2(get_column_x_pos(column), (_level->_position - _level->get_tile_pos(this) - get_tile_length() / 2.0L) / 4.0L * 2.0L - 1.0L), Vec2(0.25L, get_height()), get_tile_length() * 2.0L / _level->game->renderer->get_aspect_ratio(), 1.5L, Vec2(-1, -1), Vec2(1, 0), Colors::BLACK);
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
		_level->game->renderer->render(texture, {}, 
		random_jken == 1 ? texture->get_psize() : glm::u32vec2{1,1}, pos,
			{ 0.25,  get_height() }, {}, { 0,1 });
	}
	else if (is_state(&SingleTileCleared) && !_level->is_game_over())
	{
		const auto offset = y_offset - m_cleared_position;
		if (offset < 2.0L / 3.0L)
		{
			render_clearing(offset, &_level->txt_single_tile, pos, get_height(), _level->game->renderer.get());
		}
	}
}

void SingleTile::render_clearing(Number offset, Texture* texture, Vec2 pos, Number tile_height, Renderer* renderer)
{
	texture->tint.a = 127;
	texture->blend_mode = 1;

	const auto side_size = (1.0L / 15.0L) - offset * 0.1L;

	const std::array<Vec2, 4> positions{Vec2{},Vec2{0.0L, -tile_height * 2.0L},Vec2{0.25L, 0.0L},Vec2{-0.25L, 0.0L}};
	const std::array<Vec2, 2> sizes{Vec2{0.25L - side_size * 0.5L, tile_height * side_size},Vec2{0.25L * side_size, tile_height}};
	constexpr std::array<Vec2, 4> offsets{Vec2{0.0L,1.0L},Vec2{0.0L,-1.0L},Vec2{1.0L,1.0L},Vec2{-1.0L,1.0L}};

	for (std::uint8_t i = 0; i < 4; ++i)
		renderer->render(texture, {}, texture->get_psize(), Vec2{pos.x, pos.y} + positions[i], sizes[i/2], {}, offsets[i]);	

	texture->tint.a = 255;
	texture->blend_mode = 0;
}

			/*_level->game->renderer->render(texture, {}, texture->get_psize(), {pos.x, pos.y},
			Vec2{ 0.25L - side_size * 0.5L, get_height() * side_size } , {}, { 0,1 });
			_level->game->renderer->render(texture, {}, texture->get_psize(), {pos.x, pos.y - get_height() * 2.0L },
			Vec2{ 0.25L - side_size * 0.5L, get_height() * side_size } , {}, { 0,-1 });

			_level->game->renderer->render(texture, {}, texture->get_psize(), {pos.x + 0.25L, pos.y},
			Vec2{ 0.25L * side_size, get_height() } , {}, { 1,1 });
			_level->game->renderer->render(texture, {}, texture->get_psize(), {pos.x - 0.25L, pos.y},
			Vec2{ 0.25L * side_size, get_height() } , {}, { -1,1 });*/