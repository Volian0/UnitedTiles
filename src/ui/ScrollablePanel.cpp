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
	for (auto [finger_id, pos] : _state->touch_down)
	{
		if (in_range(pos) && !_fingers.count(finger_id))
		{
			_fingers[finger_id] = {pos.y, 0.0L};
		}
	}
	for (auto [finger_id, pos] : _state->touch_move)
	{
		if (_fingers.count(finger_id))
		{
			_fingers[finger_id].second = pos.y - _fingers[finger_id].first;
		}
	}
	for (auto [finger_id, pos] : _state->touch_up)
	{
		if (_fingers.count(finger_id))
		{
			_offset += _fingers[finger_id].second;
			_offset = std::clamp(_offset, min_offset, max_offset);
			_fingers.erase(finger_id);
		}
	}
}
