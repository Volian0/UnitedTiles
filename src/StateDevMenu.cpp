#include "StateDevMenu.h"

#include "Game.h"
#include "Renderer.h"
#include "StateSongSelection.h"
#include "StateSettings.h"

#include <SDL/SDL.h>

#include <algorithm>

StateDevMenu::StateDevMenu(Game* game_) : GameState(game_),
	_dustmotes{
		{255, 255, 255, 127},
		game_->renderer.get(),
		"note.png",
		0.125, 0.25,
		8,
		1, 4,
		0
	},
	_old_tp{ Clock::now() }
{
	_font = std::make_unique<Font>(game->renderer.get(), "roboto.ttf", 8.0L);
	_logo = std::make_unique<Texture>(game->renderer.get(), "logo.png");
	_bg = std::make_unique<Texture>(game->renderer.get(), "dev_background.png");
	_dev_button_texture = std::make_unique<Texture>(game->renderer.get(), "dev_button.png");
	_dev_button_texture->blend_mode = 1;
	std::unique_ptr<Font> small_font = std::make_unique<Font>(game->renderer.get(), "roboto.ttf", 4.0L);
	_dev_buttons.emplace_back(Vec2{}, 1, _dev_buttons.size(), "Play", _dev_button_texture.get(), _font.get(), this);
	_dev_buttons.emplace_back(Vec2{}, 1, _dev_buttons.size(), "Settings", _dev_button_texture.get(), _font.get(), this);
	_dev_buttons.emplace_back(Vec2{}, 1, _dev_buttons.size(), "Discord", _dev_button_texture.get(), _font.get(), this);
	_dev_buttons.emplace_back(Vec2{}, 1, _dev_buttons.size(), "Exit", _dev_button_texture.get(), _font.get(), this);
	soundfont = game->audio->load_soundfont("test.sf2");
	soundfont->add_event(std::chrono::time_point_cast<Clock::duration>(Clock::now() + std::chrono::duration<Number>(0.05L)), NoteEvent(NoteEvent::Type::ON, 50, 63));
	soundfont->add_event(std::chrono::time_point_cast<Clock::duration>(Clock::now() + std::chrono::duration<Number>(0.15L)), NoteEvent(NoteEvent::Type::ON, 53, 80));
	soundfont->add_event(std::chrono::time_point_cast<Clock::duration>(Clock::now() + std::chrono::duration<Number>(0.25L)), NoteEvent(NoteEvent::Type::ON, 57, 90));
	soundfont->add_event(std::chrono::time_point_cast<Clock::duration>(Clock::now() + std::chrono::duration<Number>(1.25L)), NoteEvent(NoteEvent::Type::ALL_OFF));
}

void StateDevMenu::update()
{
	Number aspect_ratio = game->renderer->get_aspect_ratio();
	Number logo_height = 0.5L * aspect_ratio;
	Number button_height = 0.125L * aspect_ratio;
	Number margin = (1.0L - logo_height - 4.0L * button_height) / 6.0L;
	_logo_position = margin + logo_height / 2.0L;
	for (uint8_t i = 0; i < _dev_buttons.size(); ++i)
	{
		_dev_buttons[i].position = { 0, (2.0L * margin + logo_height + (button_height+margin) * Number(i) + button_height/2.0L) * 2.0L - 1.0L};
		_dev_buttons[i].width = std::clamp(1.0L - 2.0L*margin/aspect_ratio, 0.5L, 1.0L);
	}

	if (_dev_buttons[0].update())
	{
		return game->change_state<StateSongSelection>();
	}
	else if (_dev_buttons[1].update())
	{
		return game->change_state<StateSettings>();
	}
	else if (_dev_buttons[2].update())
	{
		SDL_OpenURL("https://discord.gg/PpNgM9exT8");
	}
	else if (_dev_buttons[3].update())
	{
		return game->stop();
	}
	Timepoint new_tp = Clock::now();
	Number delta_time = std::chrono::duration<Number>(new_tp - _old_tp).count();
	_dustmotes.update(delta_time);
	_old_tp = new_tp;
}

void StateDevMenu::render() const
{
	game->renderer->render(_bg.get(), { 0,0 }, _bg->get_psize(), { 0,0 }, { 1,1 }, { 0,0 });
	_dustmotes.render();
	game->renderer->render(_logo.get(), { 0,0 }, _logo->get_psize(), { 0,_logo_position*2.0L-1 }, Vec2(0.5, 0.5 * game->renderer->get_aspect_ratio()), { 0,0 }, {0,0});
	for (auto& button : _dev_buttons)
	{
		button.render();
	}
}
