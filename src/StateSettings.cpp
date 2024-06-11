#include "StateSettings.h"

#include "Font.h"
#include "Game.h"
#include "Renderer.h"
#include "StateSongMenu.h"
#include "StateSoundfonts.h"

#include <algorithm>

StateSettings::StateSettings(Game* game_)
	:GameState(game_),
	_font{ game_->renderer.get(), "roboto.ttf", 6.0L },
	_bg{ game->renderer.get(), "white.png" },
	_dev_button_texture{ game->renderer.get(), "dev_button.png" },
	_b_discard{ {-0.5, 0}, 0.4L, 0, "Discard", &_dev_button_texture, &_font, this},
	_b_apply{ {0.5, 0}, 0.4L, 0, "Apply", &_dev_button_texture, &_font, this },
	_b_soundfonts{ {0.0, 0}, 0.8L, 0, "Extra Soundfonts", &_dev_button_texture, &_font, this },
	cb_u{ game->renderer.get(), "ui/checkbox0.png"},
	cb_c{ game->renderer.get(), "ui/checkbox1.png"}
{
	_bg.tint = {0xB0, 0xD0, 0xFF, 255}; 
	_dev_button_texture.blend_mode = 1;
	cb_u.blend_mode = 1;
	cb_c.blend_mode = 1;
	scrollable_panel.origin = Vec2{ 0, -1 };
	//append check boxes
	check_boxes.emplace_back(std::piecewise_construct, std::forward_as_tuple(&cb_u, &cb_c, &_font, "Stereo Audio", game->renderer.get()), std::forward_as_tuple(game->cfg->audio_stereo));
	//check_boxes.emplace_back(std::piecewise_construct, std::forward_as_tuple(&cb_u, &cb_c, &_font, "Burst particles", game->renderer.get()), std::forward_as_tuple(game->cfg->enable_particles_burst));
	//check_boxes.emplace_back(std::piecewise_construct, std::forward_as_tuple(&cb_u, &cb_c, &_font, "DustMotes particles", game->renderer.get()), std::forward_as_tuple(game->cfg->enable_particles_dustmotes));
	//check_boxes.emplace_back(std::piecewise_construct, std::forward_as_tuple(&cb_u, &cb_c, &_font, "Show debug hit markers", game->renderer.get()), std::forward_as_tuple(game->cfg->enable_hit_markers));
	//check_boxes.emplace_back(std::piecewise_construct, std::forward_as_tuple(&cb_u, &cb_c, &_font, "Show FPS", game->renderer.get()), std::forward_as_tuple(game->cfg->show_fps));
	//check_boxes.emplace_back(std::piecewise_construct, std::forward_as_tuple(&cb_u, &cb_c, &_font, "Show debug information", game->renderer.get()), std::forward_as_tuple(game->cfg->show_gameplay_debug_info));
	check_boxes.emplace_back(std::piecewise_construct, std::forward_as_tuple(&cb_u, &cb_c, &_font, "Enable V-Sync", game->renderer.get()), std::forward_as_tuple(game->cfg->enable_vsync));
	check_boxes.emplace_back(std::piecewise_construct, std::forward_as_tuple(&cb_u, &cb_c, &_font, "Auto Mode", game->renderer.get()), std::forward_as_tuple(game->cfg->god_mode));
	check_boxes.emplace_back(std::piecewise_construct, std::forward_as_tuple(&cb_u, &cb_c, &_font, "Support the game by banner ads", game->renderer.get()), std::forward_as_tuple(game->cfg->show_banner_ads));
	check_boxes.emplace_back(std::piecewise_construct, std::forward_as_tuple(&cb_u, &cb_c, &_font, "Support the game by game over ads", game->renderer.get()), std::forward_as_tuple(game->cfg->show_interstitial_ads));
	//check_boxes.emplace_back(std::piecewise_construct, std::forward_as_tuple(&cb_u, &cb_c, &_font, "Fake lag", game->renderer.get()), std::forward_as_tuple(game->cfg->fake_lag));
	//check_boxes.emplace_back(std::piecewise_construct, std::forward_as_tuple(&cb_u, &cb_c, &_font, "Smooth tempo for laps", game->renderer.get()), std::forward_as_tuple(game->cfg->three_section_mode));
	//check_boxes.emplace_back(std::piecewise_construct, std::forward_as_tuple(&cb_u, &cb_c, &_font, "Show TPS as score", game->renderer.get()), std::forward_as_tuple(game->cfg->show_tps_instead_of_score));
	//check_boxes.emplace_back(std::piecewise_construct, std::forward_as_tuple(&cb_u, &cb_c, &_font, "Always sustain notes", game->renderer.get()), std::forward_as_tuple(game->cfg->sustain_mode));
	//check_boxes.emplace_back(std::piecewise_construct, std::forward_as_tuple(&cb_u, &cb_c, &_font, "Show SDL errors", game->renderer.get()), std::forward_as_tuple(game->cfg->show_sdl_errors));
	//check_boxes.emplace_back(std::piecewise_construct, std::forward_as_tuple(&cb_u, &cb_c, &_font, "Limit note velocity", game->renderer.get()), std::forward_as_tuple(game->cfg->limit_note_velocity));

	//set correct checkbox values
	for (auto& checkbox : check_boxes)
	{
		checkbox.first._checked = checkbox.second;
		checkbox.first.spanel = &scrollable_panel;
	}
	_b_soundfonts.spanel = &scrollable_panel;
	scrollable_panel._state = this;
	scrollable_panel.max_offset = 0.0L;
}

void StateSettings::update()
{
	if (_b_apply.update())
	{
		for (auto& checkbox : check_boxes)
		{
			checkbox.second.get() = checkbox.first._checked;
		}
		game->append_cfg();
		return game->change_state<StateSongMenu>();
	}
	if (_b_discard.update())
	{
		return game->change_state<StateSongMenu>();
	}
	//set button positions
	Number button_y_pos = -1.0L + ((1.0L / 8.0L) + 0.1L) * game->renderer->get_aspect_ratio();
	Number button_height = ((1.0L / 8.0L) + 0.1L) * game->renderer->get_aspect_ratio() * 2.0L;
	scrollable_panel.position = { 0, -1.0L + button_height };
	scrollable_panel.size = { 2.0L, 2.0L };
	//scrollable_panel.min_offset = -1.75L * game->renderer->get_aspect_ratio();
	scrollable_panel.min_offset = 0.0F;
	const auto scrolled = scrollable_panel.update(game->renderer->get_aspect_ratio());
	_b_apply.position.y = button_y_pos;
	_b_discard.position.y = button_y_pos;
	for (uint8_t i = 0; i < check_boxes.size(); ++i)
	{
		check_boxes[i].first.label.position.y = -1.0L + button_height + 0.1L * game->renderer->get_aspect_ratio() + 0.2L * game->renderer->get_aspect_ratio() * Number(i) + scrollable_panel.get_offset();
		check_boxes[i].first.update(this);
	}
	_b_soundfonts.position.y = -1.0L + button_height + Number(check_boxes.size() + 1) * 0.2L * game->renderer->get_aspect_ratio() + scrollable_panel.get_offset();
	if (scrollable_panel.is_scrolled() || scrolled)
		_b_soundfonts.clear_held();
	if (_b_soundfonts.update())
	{
		return game->change_state<StateSoundfonts>();
	}
}

void StateSettings::render() const
{
	game->renderer->render(&_bg, { 0,0 }, _bg.get_psize(), { 0,0 }, { 1,1 }, { 0,0 });
	_b_apply.render();
	_b_discard.render();
	scrollable_panel.start_rendering();
	for (auto& checkbox : check_boxes)
	{
		checkbox.first.render(game->renderer.get());
	}
	_b_soundfonts.render();
	scrollable_panel.stop_rendering();
}
