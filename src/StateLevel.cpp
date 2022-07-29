#include "StateLevel.h"

#include "File.h"
#include "Path.h"
#include "Game.h"
#include "StateSongSelection.h"

#include "tiles/DoubleTile.h"
#include "tiles/EmptyTile.h"
#include "tiles/LongTile.h"
#include "tiles/SingleTile.h"
#include "tiles/SliderTile.h"

#include <algorithm>
#include <iostream>

StateLevel::StateLevel(Game* game_, const std::string& filename)
	:GameState(game_),
	score{this},
	bg{game->renderer.get(), "level_background.png"},
	tile_divider{game->renderer.get(), "tile_divider.png"},
	bg_o{game->renderer.get(), "level_background_overlay.png"},
	txt_single_tile{game->renderer.get(), "single_tile.png"},
	txt_game_over_tile{game->renderer.get(), "game_over_tile.png"},
	txt_single_tile_cleared{game->renderer.get(), "single_tile_cleared.png"},
	txt_long_tile{game->renderer.get(), "long_tile.png"},
	txt_long_tile_ext{game->renderer.get(), "long_tile_ext.png"},
	txt_long_tile_clearing{game->renderer.get(), "long_tile_clearing.png"},
	txt_long_tile_circle{game->renderer.get(), "long_tile_circle.png"},
	txt_long_tile_end{game->renderer.get(), "long_tile_end.png"},
	slider_tile{game->renderer.get(), "slider.png"},
	slider_tile_clearing{game->renderer.get(), "slider_clearing.png"},
	_debug_font{game->renderer.get(), "roboto.ttf", 4},
	_dustmotes{
	{255, 255, 255, 127},
	game_->renderer.get(),
	"glow.png",
	0.25, 0.5,
	3,
	8, 8,
	0, 2},
	_dustmotes_stars{
	{200, 255, 255, 127},
	game_->renderer.get(),
	"star.png",
	0.025, 0.05,
	8,
	2, 3,
	1},
	_burst{game_->renderer.get(), "white.png"}
{
	tile_divider.blend_mode = 1;
	txt_single_tile_cleared.blend_mode = 1;
	bg_o.blend_mode = 1;
	slider_tile.blend_mode = 1;
	slider_tile_clearing.blend_mode = 1;
	txt_long_tile_end.blend_mode = 1;
	txt_long_tile_circle.blend_mode = 1;
	tile_divider.tint = { 200, 255, 255, 80 };
	bg_o.tint = { 63, 63, 63, 15 };
	soundfont = game->audio->load_soundfont("test.sf2");
	ExtractedRes song_info_res(filename, "songs");
	auto song_info_file = open_ifile(song_info_res.get_path()).value();
	_song_info = song_info_file;
	tps = 0;
	_old_tp = new_tp = last_tempo_change = {};
	previous_position = 3.0L;
	_position = 3.0L;
	//_song_info.starting_tempo = 0.0L;
	//_song_info.acceleration_method = SongInfo::AccelerationMethod::ACCELERATION;
	//_song_info.acceleration_info.parameter = 3.0L;
}

void StateLevel::render_debug() const
{
	Texture dbg_tps{ game->renderer.get(), &_debug_font, "TPS: " + std::to_string(tps), Colors::WHITE };
	Texture dbg_tiles_n{ game->renderer.get(), &_debug_font, "Active tiles: " + std::to_string(tiles.size()), Colors::WHITE };
	Texture dbg_cleared_n{ game->renderer.get(), &_debug_font, "Cleared tiles: " + std::to_string(cleared_tiles), Colors::WHITE };

	game->renderer->render(&dbg_cleared_n, {}, dbg_cleared_n.get_psize(), Vec2(-1, 1), dbg_cleared_n.get_rsize(), {}, { -1,1 });
	game->renderer->render(&dbg_tiles_n, {}, dbg_tiles_n.get_psize(), Vec2(-1, 1 - (dbg_cleared_n.get_rsize().y) * 2.0L), dbg_tiles_n.get_rsize(), {}, { -1,1 });
	game->renderer->render(&dbg_tps, {}, dbg_tps.get_psize(), Vec2(-1, 1 - (dbg_cleared_n.get_rsize().y + dbg_tps.get_rsize().y) * 2.0L), dbg_tps.get_rsize(), {}, { -1,1 });
}

void StateLevel::queue_notes(const std::multimap<uint32_t, NoteEvent>& notes, bool forceplay_old, const std::optional<Timepoint> custom_tp)
{
	if (forceplay_old)
	{
		soundfont->play_all_events();
	}
	for (const auto& [offset, note_event] : notes)
	{
		if (_song_info.acceleration_method == SongInfo::AccelerationMethod::ACCELERATION)
		{
			const auto parameter = _song_info.acceleration_info.parameter;
			const auto start_tps = custom_tp.has_value() ? (tps + parameter * (custom_tp.value() - new_tp)) : tps;
			const auto& timepoint = custom_tp.value_or(new_tp);
			const auto desired_length = Number(offset) / Number(_song_info.note_ticks_per_single_tile);
			const auto offset_s = (std::sqrt(start_tps * start_tps + 2.0L * parameter * desired_length) - start_tps) / parameter;
			soundfont->add_event(timepoint + offset_s, note_event);
		}
		else
		{
			soundfont->add_event(custom_tp.value_or(new_tp) + 
				Number(offset) / Number(_song_info.note_ticks_per_single_tile) / tps, note_event);
		}
	}
}

bool StateLevel::force_first_interaction() const
{
	return _state == State::IDLE;
}

void StateLevel::update()
{
	new_tp = Timepoint();

	Number delta_time = new_tp - _old_tp;
	_dustmotes.update(delta_time);
	_dustmotes_stars.update(delta_time);
	_burst.update(delta_time * tps * 0.25L);
	
	_old_tp = new_tp;


	if (game_over_reset.has_value() && new_tp - game_over_reset.value() > 2.0L)
	{
		return game->change_state<StateSongSelection>();
	}

	if (game_over_scroll_to.has_value() && _state == State::GAME_OVER)
	{
		Number total_scroll_length = previous_position - game_over_scroll_to.value() - 4.0L;
		_position = previous_position - std::clamp((new_tp - game_over_reset.value()) * 4.0L, 0.0L, 1.0L) * total_scroll_length;
		for (auto& [position, tile] : tiles)
		{
			tile->y_offset = _position - position;
		}
		return;
	}

	if (_state == State::ACTIVE)
	{
		if (_song_info.acceleration_method == SongInfo::AccelerationMethod::ACCELERATION)
		{
			//TODO: Is this correct!?
			const auto starting_tempo = _song_info.starting_tempo;
			const auto total_time = (new_tp - last_tempo_change);
			tps = starting_tempo + _song_info.acceleration_info.parameter * total_time;
			_position = previous_position + total_time * (starting_tempo + _song_info.acceleration_info.parameter * 0.5L * total_time);
		}
		else
		{
			_position = previous_position + (new_tp - last_tempo_change) * tps;
		}

	}

	//sort touch down
	touch_down_sorted_positions.clear();
	for (const auto& [finger_id, touch_pos] : touch_down)
	{
		touch_down_sorted_positions.emplace_back(finger_id, touch_pos);
	}
	std::sort(touch_down_sorted_positions.begin(), touch_down_sorted_positions.end(),
		[](const std::pair<uint16_t, Vec2>& pos_a, const std::pair<uint16_t, Vec2>& pos_b) { return pos_a.second.y < pos_b.second.y; });

	//update Y offset
	for (auto& [position, tile] : tiles)
	{
		tile->y_offset = _position - position;
	}

	//update tiles
	for (const auto& [tile_pos, tile] : tiles)
	{
		if (_state == State::GAME_OVER)
		{
			break;
		}
		tile->update();
		if (game->cfg->god_mode)
		{
			if (tile->is_active() && tile->y_offset > 3.0L)
			{
				if (tile->get_info().type != TileInfo::Type::SLIDER)
				{
					if (tile->get_info().type == TileInfo::Type::DOUBLE)
					{
						tile->touch_down(0, {get_column_x_pos(tile->column) + 1.0L, 0.0L});
					}
					tile->touch_down(0, {get_column_x_pos(tile->column), 0.0L});
				}
			}
		}
	}

	//update Y offset
	//for (auto& [position, tile] : tiles)
	//{
	//	tile->y_offset = _position - position;
	//}

	if (_state == State::ACTIVE)
	{
		//delete old tiles
		for (auto it = tiles.cbegin(); it != tiles.cend();)
		{
			auto action = it->second->get_action();
			if (action == TileAction::GAME_OVER)
			{
				spawn_new_tiles();
				game_over_scroll_to = it->first;
				return game_over(it->second.get());
			}
			if (action == TileAction::DELETE)
			{
				it = tiles.erase(it);
			}
			else ++it;
		}
	}

	//spawn new tiles
	spawn_new_tiles();
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
		tile->render_bg();
		if (!game_over_scroll_to.has_value() || game_over_tile != tile.get() || (new_tp % 0.25) > 0.125)
			tile->render_fg();
	}
	//render score counter
	_burst.render();
	score.render();

	if (game->cfg->show_gameplay_debug_info) render_debug();
}

Number StateLevel::get_tile_pos(const Tile* tile_)
{
	for (const auto& [position, tile] : tiles)
	{
		if (tile.get() == tile_)
		{
			return position;
		}
	}
	return 0;
}

void StateLevel::restart_level()
{
	previous_tile.reset();
	cleared_tiles = 0;
	score.set(0);
	_state = State::IDLE;
	previous_position = 0;
	tiles.clear();
	spawned_tiles = 0;
	_position = 0;
	game_over_reset.reset();
	tps = _song_info.starting_tempo;
	soundfont->reset();
	game_over_scroll_to.reset();
	game_over_tile = nullptr;
	_old_tp = new_tp = last_tempo_change = {};
}

void StateLevel::spawn_new_tiles()
{
	if (spawned_tiles == _song_info.tiles.size())
	{
		spawned_tiles = 0;
	}
	for (uint32_t i = spawned_tiles; i < _song_info.tiles.size(); ++i)
	{
		Number total_pos = Number(total_length) / Number(_song_info.length_units_per_single_tile);
		if (_position > total_pos)
		{
			total_length += _song_info.tiles[i].length;
			switch (_song_info.tiles[i].type)
			{
			case TileInfo::Type::SINGLE:
				previous_tile = tiles.emplace(total_pos, std::make_shared<SingleTile>(this))->second;
				break;
			case TileInfo::Type::LONG:
				previous_tile = tiles.emplace(total_pos, std::make_shared<LongTile>(this))->second;
				break;
			case TileInfo::Type::DOUBLE:
				previous_tile = tiles.emplace(total_pos, std::make_shared<DoubleTile>(this))->second;
				break;
			case TileInfo::Type::EMPTY:
				previous_tile = tiles.emplace(total_pos, std::make_shared<EmptyTile>(this))->second;
				break;
			case TileInfo::Type::SLIDER:
				previous_tile = tiles.emplace(total_pos, std::make_shared<SliderTile>(this))->second;
				break;
			default: abort(); break;
			}
			previous_tile->y_offset = _position - total_pos;
			++spawned_tiles;
		}
		else break;
	}
}

void StateLevel::change_tempo(Number new_tps, const Timepoint& tp_now, Number position)
{
	if (tps == new_tps && _state == State::ACTIVE)
		return;
	std::cout << "Changing tempo from " << tps << " TPS to " << new_tps << " TPS" << std::endl;
	previous_position = position;
	tps = new_tps;
	last_tempo_change = tp_now;
	_state = State::ACTIVE;
}

void StateLevel::game_over(Tile* tile)
{
	score.silent_update();
	game_over_tile = tile;
	//change_tempo(0, new_tp, _position);
	previous_position = _position;
	_state = State::GAME_OVER;
	soundfont->play_all_events();
	soundfont->add_event(new_tp, NoteEvent(NoteEvent::Type::ALL_OFF));
	soundfont->add_event(new_tp, NoteEvent(NoteEvent::Type::ON, 48, 127));
	soundfont->add_event(new_tp, NoteEvent(NoteEvent::Type::ON, 52, 127));
	soundfont->add_event(new_tp, NoteEvent(NoteEvent::Type::ON, 55, 127));
	game_over_reset = new_tp;
}

ScoreCounter::ScoreCounter(StateLevel* level_, uint32_t init_value)
	:_font{ level_->game->renderer.get(), "roboto.ttf", 20.0L },
	_level{ level_ }
{
	set(init_value);
}

void ScoreCounter::render() const
{
	Number diff = std::clamp(0.1L - (Timepoint() - _tp_update), 0.0L, 0.1L);
	diff = diff * diff * 10.0L;
	diff = diff * 2.0L + 0.8L;
	//Number scale = std::clamp(1.0L - (Timepoint() - _tp_update), 0.8L, 1.0L);
	_texture->tint = {64,64,64,255};
	auto angle = (diff - 0.8L) * 32.0L * (((_odd / 2) % 2 == 0) ? -1.0L : 1.0L);
	if (_value >= 100)
	{
		angle *= 0.5L;
	}
	if (_value >= 10000)
	{
		angle *= 0.5L;
	}
	if (_value >= 10000000)
	{
		angle = 0.0L;
	}
	_level->game->renderer->render(_texture.get(), { 0,0 }, _texture->get_psize(), { 0.00625,-0.84+0.0125*_level->game->renderer->get_aspect_ratio() },
		{ _texture->get_rsize().x * 0.8L,_texture->get_rsize().y*diff }, { 0,-0.84 }, {}, angle);
	_texture->tint = {255,255,255,255};
	_level->game->renderer->render(_texture.get(), { 0,0 }, _texture->get_psize(), { 0,-0.84 },
		{ _texture->get_rsize().x * 0.8L,_texture->get_rsize().y*diff }, { 0.00625,-0.84+0.0125*_level->game->renderer->get_aspect_ratio() }, {}, angle);
}

void ScoreCounter::set(uint32_t value)
{
	_value = value;
	_texture = std::make_unique<Texture>(_level->game->renderer.get(), &_font, std::to_string(_value), Color{ 255, 63, 63, 255 });
	_tp_update = _level->new_tp;
	_odd ++;
}

void ScoreCounter::add(uint32_t value)
{
	set(_value + value);
	was_silent_added = false;
}

void ScoreCounter::silent_add(uint32_t value)
{
	_value += value;
	was_silent_added = true;
}

void ScoreCounter::silent_update()
{
	if (was_silent_added)
	{
		add(0);
	}
	was_silent_added = false;
}
