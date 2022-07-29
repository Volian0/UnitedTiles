#include "StateSplash.h"

#include "Game.h"
#include "Renderer.h"
#include "StateSongSelection.h"
#include "StateDevMenu.h"
#include "Colors.h"
#include <algorithm>

StateSplash::StateSplash(Game* game_) : GameState(game_)
{
	_font = std::make_unique<Font>(game->renderer.get(), "roboto.ttf", 10.0L);
	_text = std::make_unique<Texture>(game->renderer.get(), _font.get(), "Volian0 presents...", Colors::WHITE);
}

void StateSplash::update()
{
	_new_tp = Timepoint{};
	if (!_first_tp)
	{
		_first_tp = _new_tp;
	}
	if (_new_tp - _first_tp.value() > _required_time)
	{
		return game->change_state<StateDevMenu>();
	}
}

void StateSplash::render() const
{
	const Number diff = _new_tp - _first_tp.value();
	Number alpha = std::max(std::sin(diff * 1.57079632679L), 0.0L);
	const std::uint8_t c = alpha * 255.0L;
	_text->tint = Color{c, c, c, 255};
	game->renderer->render(_text.get(), {}, _text->get_psize(), {}, _text->get_rsize() * std::cbrt(diff / 2.0L), {});
}
