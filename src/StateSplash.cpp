#include "StateSplash.h"

#include "Game.h"
#include "Renderer.h"
#include "StateSongMenu.h"
#include "StateLevel.h"
#include "Colors.h"
#include "DustMotes.h"
#include <algorithm>
#include <cmath>

StateSplash::StateSplash(Game* game_) : GameState(game_),
m_logo{game->renderer.get(), "logo.png"},
m_white{game->renderer.get(), "white.png"},
m_font{game->renderer.get(), "roboto.ttf", 6.0L},
m_text{game->renderer.get(), &m_font, "v0.3.0 by Volian0", Colors::GRAY},
m_dustmotes{{32, 32, 32, 255}, game->renderer.get(), "glow.png", 1.0L, 2.0L, 8, 1.0L, 1.0L, 1.0L, 2}
{
	//m_dustmotes.dont_spawn = true;
	m_white.tint = {16, 16, 16, 255};
	//_font = std::make_unique<Font>(game->renderer.get(), "roboto.ttf", 10.0L);
	//_text = std::make_unique<Texture>(game->renderer.get(), _font.get(), "Volian0 presents...", Colors::WHITE);
}

void StateSplash::update()
{
	const auto new_tp = Timepoint{};
	const auto delta = new_tp - m_tp_now.value_or(new_tp);
	m_tp_now = new_tp;
	m_passed_time = new_tp - m_first_tp.value_or(new_tp);
	if (m_passed_time >= m_required_time)
	{
		//return game->change_state<StateLevel>(uint16_t(1));
		return game->change_state<StateSongMenu>();
	}
	if (m_passed_time > 2.0L)
	{
		m_dustmotes.dont_spawn = true;
		//m_angle = std::pow(m_passed_time - 2.0L, 4.0L) * 360.0L;
	}
	m_dustmotes.update(delta);
}

void StateSplash::render() const
{
	const Number aspect_ratio = game->renderer->get_aspect_ratio();

	game->renderer->render(&m_white, {}, m_white.get_psize(), {}, {1.0L, 1.0L}, {});
	m_dustmotes.render();
	game->renderer->render(&m_text, {}, m_text.get_psize(), {0.0L, 0.75L}, m_text.get_rsize(), {});
	game->renderer->render(&m_logo, {}, m_logo.get_psize(), {0.0L, 0.0L}, Vec2{0.5L, 0.5L * aspect_ratio}, {}, {});
	if (!m_first_tp)
	{
		m_first_tp = Timepoint{};
	}
	/*
	const Number diff = _new_tp - _first_tp.value();
	Number alpha = std::max(std::sin(diff * 1.57079632679L), 0.0L);
	const std::uint8_t c = alpha * 255.0L;
	_text->tint = Color{c, c, c, 255};
	game->renderer->render(_text.get(), {}, _text->get_psize(), {}, _text->get_rsize() * std::cbrt(diff / 2.0L), {});
	*/
}
