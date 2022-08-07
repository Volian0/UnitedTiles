#include "StateSettings.h"

#include "Font.h"
#include "Game.h"
#include "Renderer.h"
#include "StateDevMenu.h"

#include <algorithm>

StateSettings::StateSettings(Game* game_)
	:GameState(game_),
	_font{ game_->renderer.get(), "roboto.ttf", 8.0L },
	_bg{ game->renderer.get(), "dev_background.png" },
	_dev_button_texture{ game->renderer.get(), "dev_button.png" },
	_b_discard{ {-0.5, 0}, 0.4L, 0, "Discard", &_dev_button_texture, &_font, this},
	_b_apply{ {0.5, 0}, 0.4L, 0, "Apply", &_dev_button_texture, &_font, this },
	cb_u{ game->renderer.get(), "ui/checkbox0.png"},
	cb_c{ game->renderer.get(), "ui/checkbox1.png"}
{
	_dev_button_texture.blend_mode = 1;
	cb_u.blend_mode = 1;
	cb_c.blend_mode = 1;
	scrollable_panel.origin = Vec2{ 0, -1 };
	//append check boxes
	check_boxes.emplace_back(std::piecewise_construct, std::forward_as_tuple(&cb_u, &cb_c, &_font, "Stereo audio", game->renderer.get()), std::forward_as_tuple(game->cfg->audio_stereo));
	check_boxes.emplace_back(std::piecewise_construct, std::forward_as_tuple(&cb_u, &cb_c, &_font, "Burst particles", game->renderer.get()), std::forward_as_tuple(game->cfg->enable_particles_burst));
	check_boxes.emplace_back(std::piecewise_construct, std::forward_as_tuple(&cb_u, &cb_c, &_font, "DustMotes particles", game->renderer.get()), std::forward_as_tuple(game->cfg->enable_particles_dustmotes));
	check_boxes.emplace_back(std::piecewise_construct, std::forward_as_tuple(&cb_u, &cb_c, &_font, "Show debug hit markers", game->renderer.get()), std::forward_as_tuple(game->cfg->enable_hit_markers));
	check_boxes.emplace_back(std::piecewise_construct, std::forward_as_tuple(&cb_u, &cb_c, &_font, "Show FPS", game->renderer.get()), std::forward_as_tuple(game->cfg->show_fps));
	check_boxes.emplace_back(std::piecewise_construct, std::forward_as_tuple(&cb_u, &cb_c, &_font, "Show debug information", game->renderer.get()), std::forward_as_tuple(game->cfg->show_gameplay_debug_info));
	check_boxes.emplace_back(std::piecewise_construct, std::forward_as_tuple(&cb_u, &cb_c, &_font, "Enable V-Sync", game->renderer.get()), std::forward_as_tuple(game->cfg->enable_vsync));
	check_boxes.emplace_back(std::piecewise_construct, std::forward_as_tuple(&cb_u, &cb_c, &_font, "God mode", game->renderer.get()), std::forward_as_tuple(game->cfg->god_mode));
	check_boxes.emplace_back(std::piecewise_construct, std::forward_as_tuple(&cb_u, &cb_c, &_font, "Fake lag", game->renderer.get()), std::forward_as_tuple(game->cfg->fake_lag));
	//set correct checkbox values
	for (auto& checkbox : check_boxes)
	{
		checkbox.first._checked = checkbox.second;
	}
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
		return game->change_state<StateDevMenu>();
	}
	if (_b_discard.update())
	{
		return game->change_state<StateDevMenu>();
	}
	//set button positions
	Number button_y_pos = -1.0L + ((1.0L / 8.0L) + 0.1L) * game->renderer->get_aspect_ratio();
	Number button_height = ((1.0L / 8.0L) + 0.1L) * game->renderer->get_aspect_ratio() * 2.0L;
	scrollable_panel.position = { 0, -1.0L + button_height };
	scrollable_panel.size = { 2.0L, 2.0L };
	scrollable_panel.min_offset = -1.0L;
	scrollable_panel.update();
	_b_apply.position.y = button_y_pos;
	_b_discard.position.y = button_y_pos;
	for (uint8_t i = 0; i < check_boxes.size(); ++i)
	{
		check_boxes[i].first.label.position.y = -1.0L + button_height + 0.1L * game->renderer->get_aspect_ratio() + 0.2L * game->renderer->get_aspect_ratio() * Number(i) + scrollable_panel.get_offset();
		check_boxes[i].first.update(this);
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
	scrollable_panel.stop_rendering();
}
