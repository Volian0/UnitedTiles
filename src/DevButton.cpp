#include "DevButton.h"

#include "GameState.h"
#include "Renderer.h"
#include "Texture.h"
#include "Game.h"
#include "Colors.h"
#include "ui/ScrollablePanel.h"

DevButton::DevButton(Vec2 position_, Number width_, uint32_t id_, const std::string& text, Texture* texture, Font* font, GameState* state, Number t_corner_size)
	:position{position_},
	width{width_},
	id{id_},
	_texture{texture},
	_state{state},
	_font{font},
	m_corner_size{t_corner_size}
{
	set_text(text);
	_text_texture->tint = Color{40, 40, 40, 255};
	_texture->tint = Color{0xD7, 0xE7, 0xFE, 255};
}

bool DevButton::update()
{
	_pressed = false;
	for (const auto& touch_event : _state->touch_events)
	{
		if (touch_event.type == TouchEvent::Type::DOWN)
		{
			if (!is_held() && in_range(touch_event.position))
			{
				_state->game->sfx_tick->play(0);
				_held_finger_id.emplace(touch_event.finger_id);
			}
		}
		else if (touch_event.type == TouchEvent::Type::UP)
		{
			if (is_held() && touch_event.finger_id == _held_finger_id.value())
			{
				_held_finger_id.reset();
				if (in_range(touch_event.position))
				{
					_state->game->sfx_press->play(0);
					_pressed = true;
				}
			}
		}
	}
	return _pressed;
}

void DevButton::render() const
{
	if (is_held())
	{
		_text_texture->tint = Colors::BLACK;
		_texture->tint = Color{192, 192, 192, 255};
	}

	Renderer* renderer = _state->game->renderer.get();
	Vec2 corner_size(m_corner_size, m_corner_size * renderer->get_aspect_ratio());

	renderer->render(_texture, { 0,0 }, _texture->get_psize(), Vec2(-width, 0) + position, corner_size, { 0,0 }, { -1,1 }, 0, 0);
	renderer->render(_texture, { 0,0 }, _texture->get_psize(), Vec2(-width, 0) + position, corner_size, { 0,0 }, { -1,-1 }, 0, 2);
	renderer->render(_texture, { 0,0 }, _texture->get_psize(), Vec2(width, 0) + position, corner_size, { 0,0 }, { 1,1 }, 0, 1);
	renderer->render(_texture, { 0,0 }, _texture->get_psize(), Vec2(width, 0) + position, corner_size, { 0,0 }, { 1,-1 }, 0, 3);

	renderer->render(_texture, { _texture->get_psize().x - 1,0 }, { 1, _texture->get_psize().y }, Vec2(0, 0) + position, Vec2(width - corner_size.x * 2, corner_size.y), { 0,0 }, { 0,-1 }, 0, 2);
	renderer->render(_texture, { _texture->get_psize().x - 1,0 }, { 1, _texture->get_psize().y }, Vec2(0, 0) + position, Vec2(width - corner_size.x * 2, corner_size.y), { 0,0 }, { 0,1 }, 0, 1);

	_state->game->renderer->render(_text_texture.get(), { 0,0 }, _text_texture->get_psize(), position, _text_texture->get_rsize(), { 0,0 });

	_text_texture->tint = Color{40, 40, 40, 255};
	_texture->tint = Color{0xD7, 0xE7, 0xFE, 255};
}

void DevButton::set_text(const std::string& text)
{
	_text_texture = std::make_unique<Texture>(_state->game->renderer.get(), _font, text, Colors::WHITE);
	_text_texture->tint = Color{40, 40, 40, 255};
}

bool DevButton::is_pressed() const
{
	return _pressed;
}

bool DevButton::is_held() const
{
	return _held_finger_id.has_value();
}

bool DevButton::in_range(Vec2 pos) const
{
	if (spanel && !spanel->in_range(pos))
	{
		return false;
	}
	Vec2 diff = glm::abs(position - pos);
	return (diff.x <= width && diff.y <= m_corner_size * 2.0L * _state->game->renderer->get_aspect_ratio());
}
