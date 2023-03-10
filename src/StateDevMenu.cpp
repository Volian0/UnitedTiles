#include "StateDevMenu.h"

#include "Game.h"
#include "Renderer.h"
#include "StateSongSelection.h"
#include "StateSongMenu.h"
#include "StateSettings.h"
#include "RNG.h"

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
	}
{
	_font = std::make_unique<Font>(game->renderer.get(), "roboto.ttf", 8.0L);
	{
		const auto _font2 = std::make_unique<Font>(game->renderer.get(), "roboto.ttf", 4.0L);
		_text_version = std::make_unique<Texture>(game->renderer.get(), _font2.get(), "v0.2", Colors::GRAY);
		_text_volian = std::make_unique<Texture>(game->renderer.get(), _font2.get(), "by Volian0", Colors::GRAY);
	}
	_logo = std::make_unique<Texture>(game->renderer.get(), "logo.png");
	_bg = std::make_unique<Texture>(game->renderer.get(), "dev_background.png");
	_dev_button_texture = std::make_unique<Texture>(game->renderer.get(), "dev_button.png");
	_dev_button_texture->blend_mode = 1;
	std::unique_ptr<Font> small_font = std::make_unique<Font>(game->renderer.get(), "roboto.ttf", 4.0L);
	_dev_buttons.emplace_back(Vec2{}, 1, _dev_buttons.size(), "Play", _dev_button_texture.get(), _font.get(), this);
	_dev_buttons.emplace_back(Vec2{}, 1, _dev_buttons.size(), "Settings", _dev_button_texture.get(), _font.get(), this);
	_dev_buttons.emplace_back(Vec2{}, 1, _dev_buttons.size(), "Discord", _dev_button_texture.get(), _font.get(), this);
	_dev_buttons.emplace_back(Vec2{}, 1, _dev_buttons.size(), "Exit", _dev_button_texture.get(), _font.get(), this);
	//soundfont = game->audio->load_soundfont("test.sf2");
	/*Timepoint now;
	const uint8_t offset = RNG::integer(0, 2) * 12;
	soundfont->add_event(now, NoteEvent(NoteEvent::Type::ON, 65 - offset, 127));

	soundfont->add_event(now + 0.1, NoteEvent(NoteEvent::Type::ALL_OFF));
	soundfont->add_event(now + 0.1, NoteEvent(NoteEvent::Type::ON, 63 - offset, 120));

	soundfont->add_event(now + 0.2, NoteEvent(NoteEvent::Type::ALL_OFF));
	soundfont->add_event(now + 0.2, NoteEvent(NoteEvent::Type::ON, 65 - offset, 127));

	soundfont->add_event(now + 0.5, NoteEvent(NoteEvent::Type::ALL_OFF));
	soundfont->add_event(now + 0.5, NoteEvent(NoteEvent::Type::ON, 63 - offset, 120));

	soundfont->add_event(now + 0.8, NoteEvent(NoteEvent::Type::ALL_OFF));
	soundfont->add_event(now + 0.8, NoteEvent(NoteEvent::Type::ON, 65 - offset, 127));

	soundfont->add_event(now + 0.9L, NoteEvent(NoteEvent::Type::ALL_OFF));*/
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
		return game->change_state<StateSongMenu>();
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
	Timepoint new_tp;
	Number delta_time = new_tp - _old_tp;
	_dustmotes.update(delta_time);
	_old_tp = new_tp;
}

void StateDevMenu::render() const
{
	game->renderer->render(_bg.get(), { 0,0 }, _bg->get_psize(), { 0,0 }, { 1,1 }, { 0,0 });
	_dustmotes.render();
	game->renderer->render(_logo.get(), { 0,0 }, _logo->get_psize(), { 0,_logo_position*2.0L-1 }, Vec2(0.5, 0.5 * game->renderer->get_aspect_ratio()), { 0,0 }, {0,0});
	game->renderer->render(_text_volian.get(), { 0,0 }, _text_volian->get_psize(), {1.0L - _text_volian->get_rsize().x,-1.0L + _text_volian->get_rsize().y}, _text_volian->get_rsize(), { 0,0 }, {0,0});
	game->renderer->render(_text_version.get(), { 0,0 }, _text_version->get_psize(), {1.0L - _text_version->get_rsize().x,-1.0L + _text_volian->get_rsize().y * 3.0L}, _text_version->get_rsize(), { 0,0 }, {0,0});
	for (auto& button : _dev_buttons)
	{
		button.render();
	}
}
