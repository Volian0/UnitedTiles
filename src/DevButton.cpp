#include "DevButton.h"

#include "GameState.h"
#include "Renderer.h"
#include "Texture.h"
#include "Game.h"
#include "Color.h"

DevButton::DevButton(Vec2 position_, Number width_, uint32_t id_, const std::string& text, Texture* texture, Font* font, GameState* state)
	:position{position_},
	width{width_},
	id{id_},
	_texture{texture},
	_state{state},
	_font{font}
{
	set_text(text);
}

bool DevButton::update()
{
	_pressed = false;
	if (is_held())
	{
		uint16_t finger_id = _held_finger_id.value();
		if (_state->touch_up.count(finger_id))
		{
			_held_finger_id.reset();
			if (in_range(_state->touch_up[finger_id]))
			{
				_pressed = true;
			}
		}
	}
	else
	{
		for (auto& [key, value] : _state->touch_down)
		{
			if (in_range(value))
			{
				_held_finger_id.emplace(key);
				break;
			}
		}
	}
	return _pressed;
}

void DevButton::render() const
{
	if (is_held())
	{
		_text_texture->tint = Color::BLACK;
		_texture->tint = Color::GRAY;
	}

	Renderer* renderer = _state->game->renderer.get();
	Vec2 corner_size(0.0625L, 0.0625L * renderer->get_aspect_ratio());

	renderer->render(_texture, { 0,0 }, _texture->get_psize(), Vec2(-width, 0) + position, corner_size, { 0,0 }, { -1,1 }, 0, 0);
	renderer->render(_texture, { 0,0 }, _texture->get_psize(), Vec2(-width, 0) + position, corner_size, { 0,0 }, { -1,-1 }, 0, 2);
	renderer->render(_texture, { 0,0 }, _texture->get_psize(), Vec2(width, 0) + position, corner_size, { 0,0 }, { 1,1 }, 0, 1);
	renderer->render(_texture, { 0,0 }, _texture->get_psize(), Vec2(width, 0) + position, corner_size, { 0,0 }, { 1,-1 }, 0, 3);

	renderer->render(_texture, { _texture->get_psize().x - 1,0 }, { 1, _texture->get_psize().y }, Vec2(0, 0) + position, Vec2(width - corner_size.x * 2, corner_size.y), { 0,0 }, { 0,-1 }, 0, 2);
	renderer->render(_texture, { _texture->get_psize().x - 1,0 }, { 1, _texture->get_psize().y }, Vec2(0, 0) + position, Vec2(width - corner_size.x * 2, corner_size.y), { 0,0 }, { 0,1 }, 0, 1);

	_state->game->renderer->render(_text_texture.get(), { 0,0 }, _text_texture->get_psize(), position, _text_texture->get_rsize(), { 0,0 });

	_text_texture->tint = Color::WHITE;
	_texture->tint = Color::WHITE;
}

void DevButton::set_text(const std::string& text)
{
	_text_texture = std::make_unique<Texture>(_state->game->renderer.get(), _font, text, Color::WHITE);
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
	Vec2 diff = glm::abs(position - pos);
	return (diff.x <= width && diff.y <= 0.125L * _state->game->renderer->get_aspect_ratio());
}
