#include "StateLevel.h"

#include "File.h"
#include "Path.h"
#include "Game.h"

#include <iostream>

StateLevel::StateLevel(Game* game_, const std::string& filename)
	:GameState(game_),
	bg{game->renderer.get(), "level_background.png"},
	tile_divider{game->renderer.get(), "tile_divider.png"},
	bg_o{game->renderer.get(), "level_background_overlay.png"},
	txt_single_tile{game->renderer.get(), "single_tile.png"},
	_dustmotes{
	{255, 255, 255, 127},
	game_->renderer.get(),
	"glow.png",
	0.5, 1,
	2,
	8, 8,
	0, 2},
	_dustmotes_stars{
	{200, 255, 255, 127},
	game_->renderer.get(),
	"star.png",
	0.05, 0.1,
	8,
	1, 2,
	1}
{
	tile_divider.blend_mode = 1;
	bg_o.blend_mode = 1;
	tile_divider.tint = { 200, 255, 255, 63 };
	bg_o.tint = { 63, 63, 63, 31 };
	soundfont = game->audio->load_soundfont("test.sf2");
	ExtractedRes song_info_res(filename, "songs");
	auto song_info_file = open_ifile(song_info_res.get_path()).value();
	_song_info = song_info_file;
	tps = _song_info.starting_tempo;
}

void StateLevel::update()
{
	Timepoint new_tp = Clock::now();
	Number delta_time = std::chrono::duration<Number>(new_tp - _old_tp).count();
	_dustmotes.update(delta_time);
	_dustmotes_stars.update(delta_time);
	_old_tp = new_tp;
	//
	_position = previous_position + std::chrono::duration<Number>(new_tp - last_tempo_change).count() * tps;
	//delete old tiles
	//test collision
	//spawn new tiles
	uint64_t total_length = 0;
	for (uint32_t i = 0; i < _song_info.tiles.size(); ++i)
	{
		Number total_pos = Number(total_length) / Number(_song_info.length_units_per_single_tile);
		total_length += _song_info.tiles[i].length;
		if (i >= spawned_tiles)
		{
			if (_position > total_pos)
			{
				tiles.emplace(total_pos, Tile{ i, i%2==0 ? FAR_LEFT : MID_RIGHT, false });
				++spawned_tiles;
				std::cout << "tile spawned!" << std::endl;
			}
			else
			{
				break;
			}
		}
	}
}

void StateLevel::render() const
{
	game->renderer->render(&bg, {}, bg.get_psize(), {}, { 1,1 }, {});
	_dustmotes_stars.render();
	_dustmotes.render();
	game->renderer->render(&bg_o, {}, bg_o.get_psize(), {}, { 1,1 }, {});
	game->renderer->render(&tile_divider, {}, tile_divider.get_psize(), {}, { 0.01,1 }, {});
	game->renderer->render(&tile_divider, {}, tile_divider.get_psize(), {-0.5,0}, { 0.01,1 }, {});
	game->renderer->render(&tile_divider, {}, tile_divider.get_psize(), {0.5,0}, { 0.01,1 }, {});
	//render tiles
	for (const auto& [position, tile] : tiles)
	{
		const auto& tile_info = _song_info.tiles[tile.tile_id];
		if (tile_info.type == TileInfo::Type::SINGLE)
		{
			Vec2 pos;
			pos.y = (_position - position) / 4.0L * 2.0L - 1.0L;
			if (tile.column == FAR_LEFT)
				pos.x = -0.75;
			else if (tile.column == MID_LEFT)
				pos.x = -0.25;
			else if (tile.column == MID_RIGHT)
				pos.x = 0.25;
			else if (tile.column == FAR_RIGHT)
				pos.x = 0.75;
			game->renderer->render(&txt_single_tile, {}, txt_single_tile.get_psize(), pos,
				{ 0.25,Number(tile_info.length) / Number(_song_info.length_units_per_single_tile) / 4.0L }, {}, {0,1});
		}
	}
}

void StateLevel::change_tempo(Number new_tps, const Timepoint& tp_now, Number position)
{
	previous_position = position;
	tps = new_tps;
	last_tempo_change = tp_now;
	//std::chrono::duration<Number>(tp_now - last_tempo_change).count() * tps;
}
