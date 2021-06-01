#include "StateLevel.h"

#include "File.h"
#include "Path.h"
#include "Game.h"
#include "StateSongSelection.h"

#include <iostream>
#include <algorithm>

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
	txt_single_tile_cleared.blend_mode = 1;
	bg_o.blend_mode = 1;
	txt_long_tile_end.blend_mode = 1;
	txt_long_tile_circle.blend_mode = 1;
	tile_divider.tint = { 200, 255, 255, 63 };
	bg_o.tint = { 63, 63, 63, 31 };
	soundfont = game->audio->load_soundfont("test.sf2");
	ExtractedRes song_info_res(filename, "songs");
	auto song_info_file = open_ifile(song_info_res.get_path()).value();
	_song_info = song_info_file;
	tps = _song_info.starting_tempo;
	_old_tp = new_tp = last_tempo_change = Clock::now();
}

void StateLevel::queue_notes(const std::multimap<uint32_t, NoteEvent>& notes, bool forceplay_old, const std::optional<Timepoint> custom_tp)
{
	/*std::cout << "== = == Queue started == = ==" << std::endl;
	for (const auto& [offset, note_event] : notes)
	{
		std::cout << offset << " | ";
		std::cout << (note_event.type == NoteEvent::Type::ON ? "ON " : "OFF") << ", ";
		std::cout << unsigned(note_event.key);
		if (note_event.type == NoteEvent::Type::ON)
			std::cout << ", " << unsigned(note_event.velocity);
		std::cout << std::endl;
	}
	std::cout << std::endl;*/
	if (forceplay_old)
	{
		soundfont->play_all_events();
	}
	for (const auto& [offset, note_event] : notes)
	{
		soundfont->add_event(std::chrono::time_point_cast<Clock::duration>(custom_tp.value_or(new_tp) + std::chrono::duration<Number>(
			Number(offset) / Number(_song_info.note_ticks_per_single_tile) / tps)), note_event);
	}
}

void StateLevel::update()
{
	new_tp = Clock::now();

	if (game_over_scroll_to.has_value() && _state == State::GAME_OVER)
	{
		Number total_scroll_length = previous_position - game_over_scroll_to.value() - 4.0L;
		_position = previous_position - std::clamp(std::chrono::duration<Number>(new_tp - game_over_reset.value()).count() * 4.0L, 0.0L, 1.0L) * total_scroll_length;
	}

	if (game_over_reset.has_value() && new_tp - game_over_reset.value() > std::chrono::seconds(2))
	{
		//return restart_level();
		return game->change_state<StateSongSelection>();
	}

	Number delta_time = std::chrono::duration<Number>(new_tp - _old_tp).count();
	_dustmotes.update(delta_time);
	_dustmotes_stars.update(delta_time);
	_old_tp = new_tp;
	//
	if (_state != State::ACTIVE)
	{
		return;
	}
	//
	_position = previous_position + std::chrono::duration<Number>(new_tp - last_tempo_change).count() * tps;
	//delete old tiles
	for (auto it = tiles.cbegin(); it != tiles.cend();)
	{
		if (it->second->should_be_cleared(_position - it->first) && !it->second->is_cleared())
		{
			game_over_scroll_to = it->first;
			return game_over(it->second.get());
		}
		if (it->second->should_die(_position - it->first))
		{
			it = tiles.erase(it);
		}
		else ++it;
	}
	//test collision
	for (const auto& [tile_pos, tile] : tiles)
	{
		for (const auto& [finger_id, touch_pos] : touch_down)
		{
			if (!tile->touch_down(finger_id, { touch_pos.x, (_position - tile_pos) - (touch_pos.y + 1.0L) / 2.0L * 4.0L }))
				return game_over(tile.get());
		}
		for (const auto& [finger_id, touch_pos] : touch_move)
		{
			if (!tile->touch_move(finger_id, { touch_pos.x, (_position - tile_pos) - (touch_pos.y + 1.0L) / 2.0L * 4.0L }))
				return game_over(tile.get());
		}
		for (const auto& [finger_id, touch_pos] : touch_up)
		{
			if (!tile->touch_up(finger_id,   { touch_pos.x, (_position - tile_pos) - (touch_pos.y + 1.0L) / 2.0L * 4.0L }))
				return game_over(tile.get());
		}
	}
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
				switch (_song_info.tiles[i].type)
				{
				case TileInfo::Type::SINGLE:
					previous_tile = tiles.emplace(total_pos, std::make_shared<SingleTile>(i, this))->second;
					break;
				case TileInfo::Type::LONG:
					previous_tile = tiles.emplace(total_pos, std::make_shared<LongTile>(i, this))->second;
					break;
				case TileInfo::Type::DOUBLE:
					previous_tile = tiles.emplace(total_pos, std::make_shared<DoubleTile>(i, this))->second;
					break;
				default: abort(); break;
				}
				++spawned_tiles;
			}
			else break;
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
		if (!game_over_scroll_to.has_value() || game_over_tile != tile.get() || (new_tp.time_since_epoch() % std::chrono::milliseconds(250)) > std::chrono::milliseconds(125))
			tile->render(_position - position);
	}
	//render score counter
	score.render();
}

void StateLevel::restart_level()
{
	previous_tile.reset();
	cleared_tiles = 0;
	score.set(0);
	_state = State::ACTIVE;
	previous_position = 0;
	tiles.clear();
	spawned_tiles = 0;
	_position = 0;
	game_over_reset.reset();
	tps = _song_info.starting_tempo;
	soundfont->reset();
	game_over_scroll_to.reset();
	game_over_tile = nullptr;
	_old_tp = new_tp = last_tempo_change = Clock::now();
}

void StateLevel::change_tempo(Number new_tps, const Timepoint& tp_now, Number position)
{
	previous_position = position;
	tps = new_tps;
	last_tempo_change = tp_now;
	//std::chrono::duration<Number>(tp_now - last_tempo_change).count() * tps;
}

void StateLevel::game_over(Tile* tile)
{
	game_over_tile = tile;
	change_tempo(0, new_tp, _position);
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
	Number scale = std::clamp(1.0L - std::chrono::duration<Number>(Clock::now() - _tp_update).count(), 0.8L, 1.0L);
	_level->game->renderer->render(_texture.get(), { 0,0 }, _texture->get_psize(), { 0,-0.84 },
		{ _texture->get_rsize().x * 0.8L,_texture->get_rsize().y*scale }, { 0,0 });
}

void ScoreCounter::set(uint32_t value)
{
	_value = value;
	_texture = std::make_unique<Texture>(_level->game->renderer.get(), &_font, std::to_string(_value), glm::u8vec4{ 255, 63, 63, 255 });
	_tp_update = _level->new_tp;
}

void ScoreCounter::add(uint32_t value)
{
	set(_value + value);
}
