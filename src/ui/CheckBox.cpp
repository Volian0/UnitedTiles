#include "CheckBox.h"
#include "../Game.h"

CheckBox::CheckBox(Texture* txt_u, Texture* txt_c, Font* font_, const std::string& label_text, Renderer* renderer_, Vec2 position_, bool is_checked_)
	:_txt_u{txt_u}, _txt_c{txt_c},
	_checked{is_checked_},
	label{ label_text, 0.9L, {-0.8L, position_.y}, {-1.0L, 0.0L}, font_, renderer_ }
{
}

bool CheckBox::update(GameState* state)
{
	for (const auto& touch_event : state->touch_events)
	{
		if (spanel && !spanel->in_range(touch_event.position))
		{
			continue;
		}
		if (touch_event.type != TouchEvent::Type::DOWN)
			continue;
		const auto& pos = touch_event.position;
		if (pos.x < -0.8L && std::abs(pos.y - label.position.y) < 0.1L * state->game->renderer->get_aspect_ratio())
		{
			state->game->sfx_tick->play(0);
			_checked = !_checked;
			break;
		}
	}
	return _checked;
}

bool CheckBox::is_checked() const
{
	return _checked;
}

void CheckBox::render(Renderer* renderer) const
{
	Texture* txt = _checked ? _txt_c : _txt_u;
	renderer->render(txt, {}, txt->get_psize(), { -0.9L, label.position.y }, { 0.1L, 0.1L * renderer->get_aspect_ratio() }, {});
	label.render(renderer);
}