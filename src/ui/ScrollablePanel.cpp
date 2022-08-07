#include "ScrollablePanel.h"

#include "../GameState.h"
#include "../Game.h"
#include "../Renderer.h"

#include <algorithm>

void ScrollablePanel::start_rendering() const
{
	_state->game->renderer->set_clip_rect(position, size, origin);
}

void ScrollablePanel::stop_rendering() const
{
	_state->game->renderer->unset_clip();
}

Number ScrollablePanel::get_offset() const
{
	Number offset = _offset;
	for (const auto& f : _fingers)
	{
		offset += f.second.second;
	}
	return std::clamp(offset, min_offset, max_offset);
}

void ScrollablePanel::update()
{
	for (const auto& touch_event : _state->touch_events)
	{
		const auto finger_id = touch_event.finger_id;
		const auto& pos = touch_event.position;
		if (touch_event.type == TouchEvent::Type::DOWN)
		{
			if (in_range(pos) && !_fingers.count(finger_id))
			{
				_fingers[finger_id] = {pos.y, 0.0L};
			}
		}
		else if (touch_event.type == TouchEvent::Type::MOVE)
		{
			if (_fingers.count(finger_id))
			{
				_fingers[finger_id].second = pos.y - _fingers[finger_id].first;
			}
		}
		else if (touch_event.type == TouchEvent::Type::UP)
		{
			if (_fingers.count(finger_id))
			{
				_offset += _fingers[finger_id].second;
				_offset = std::clamp(_offset, min_offset, max_offset);
				_fingers.erase(finger_id);
			}
		}
	}
}
