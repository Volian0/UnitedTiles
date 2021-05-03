#include "StateSettings.h"

#include "Font.h"
#include "Game.h"
#include "Renderer.h"
#include "StateDevMenu.h"

#include <algorithm>

StateSettings::StateSettings(Game* game_)
	:GameState(game_)
{
	_font = std::make_unique<Font>(game->renderer.get(), "roboto.ttf", 8.0L);
	_bg = std::make_unique<Texture>(game->renderer.get(), "dev_background.png");
	_dev_button_texture = std::make_unique<Texture>(game->renderer.get(), "dev_button.png");
	_dev_button_texture->blend_mode = 1;
	std::unique_ptr<Font> small_font = std::make_unique<Font>(game->renderer.get(), "roboto.ttf", 4.0L);
	_dev_buttons.emplace_back(Vec2{}, 1, _dev_buttons.size(), "Return to menu", _dev_button_texture.get(), _font.get(), this);
	_dev_buttons.emplace_back(Vec2{}, 1, _dev_buttons.size(), game->renderer->vsync ? "V-sync: ON" : "V-sync: OFF", _dev_button_texture.get(), _font.get(), this);
	_dev_buttons.emplace_back(Vec2{}, 1, _dev_buttons.size(), "Sample rate: 44100", _dev_button_texture.get(), _font.get(), this);
	_dev_buttons.emplace_back(Vec2{}, 1, _dev_buttons.size(), "Channels: STEREO", _dev_button_texture.get(), _font.get(), this);
	_dev_buttons.emplace_back(Vec2{}, 1, _dev_buttons.size(), "Reset to default", _dev_button_texture.get(), _font.get(), this);
	_dev_buttons.emplace_back(Vec2{}, 1, _dev_buttons.size(), "Clear user data", _dev_button_texture.get(), _font.get(), this);
}

void StateSettings::update()
{
	Number aspect_ratio = game->renderer->get_aspect_ratio();
	Number button_height = 0.125L * aspect_ratio;
	Number margin = (1.0L - Number(_dev_buttons.size()) * button_height) / (1.0L + Number(_dev_buttons.size()));
	for (uint8_t i = 0; i < _dev_buttons.size(); ++i)
	{
		_dev_buttons[i].position = { 0, (margin + (button_height + margin) * Number(i) + button_height / 2.0L) * 2.0L - 1.0L };
		_dev_buttons[i].width = std::clamp(1.0L - 2.0L * margin / aspect_ratio, 0.8L, 1.0L);
	}
	if (_dev_buttons[0].update())
	{
		return game->change_state<StateDevMenu>();
	}
	if (_dev_buttons[1].update())
	{
		game->renderer->vsync = !game->renderer->vsync;
		game->renderer->reload();
		return game->change_state<StateSettings>();
	}
	if (_dev_buttons[2].update())
	{

	}
	if (_dev_buttons[3].update())
	{

	}
	if (_dev_buttons[4].update())
	{
		game->renderer->vsync = false;
		game->renderer->reload();
		return game->change_state<StateSettings>();
	}
	if (_dev_buttons[5].update())
	{

	}
}

void StateSettings::render() const
{
	game->renderer->render(_bg.get(), { 0,0 }, _bg->get_psize(), { 0,0 }, { 1,1 }, { 0,0 });
	for (auto& button : _dev_buttons)
	{
		button.render();
	}
}
