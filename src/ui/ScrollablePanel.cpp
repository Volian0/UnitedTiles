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
	/*Number offset = _offset;
	for (const auto& f : _fingers)
	{
		offset += f.second.second;
	}
	return std::clamp(offset, min_offset, max_offset);*/
	//const Number limit = 0.125L * t_aspect_ratio;
	//if ()
	return _offset;
}

bool ScrollablePanel::update(Number t_aspect_ratio)
{
	const Timepoint new_tp{};
	const Number delta_time = new_tp - _old_tp;
	_old_tp = new_tp;
	bool stared_scrolling = false;
	_offset = std::clamp(_offset, min_offset, max_offset); //handle new min_offset and max_offset
	for (auto& touch_event : _state->touch_events)
	{
		const auto finger_id = touch_event.finger_id;
		const auto& pos = touch_event.position;
		if (touch_event.type == TouchEvent::Type::DOWN)
		{
			if (in_range(pos))
			{
				_last_finger_position = {finger_id, pos.y};
				last_y_pos = pos.y;
				tp_changes.clear();
				velocity.reset();
			}
		}
		else if (!_last_finger_position || _last_finger_position->first != finger_id)
		{
			//do nothing
		}
		else if (touch_event.type == TouchEvent::Type::MOVE)
		{
			if (last_y_pos)
			{
				const Number margin = 0.125L * t_aspect_ratio;
				if (std::abs(pos.y - *last_y_pos) < margin)
				{
					continue;
				}
				_last_finger_position->second += (pos.y - *last_y_pos) < 0.0L ? -margin : margin;
				last_y_pos.reset();
				stared_scrolling = true;
			}
			_offset = std::clamp(_offset - (_last_finger_position->second - pos.y), min_offset, max_offset);
			tp_changes.emplace(Timepoint{}, (_last_finger_position->second - pos.y));
			_last_finger_position->second = pos.y;
		}
		else if (touch_event.type == TouchEvent::Type::UP)
		{
			if (!last_y_pos) {
				touch_event.position.y = -std::numeric_limits<Number>::infinity();
				//handle tp_changes here
				Number total_velocity = 0.0L;
				for (const auto& [tp, offset] : tp_changes)
				{
					total_velocity += std::pow(1.0L - std::clamp(new_tp - tp, 0.0L, 1.0L), 4.0L) * offset * 4.0L;
				}
				if (total_velocity > 0.0L)
				{
					velocity = total_velocity;
					velmul = 1.0L;
				}
				else
				{
					velocity = -total_velocity;
					velmul = -1.0L;
				}
			}
			last_y_pos.reset();
			_last_finger_position.reset();
			tp_changes.clear();
		}
	}
	if (!tp_changes.empty())
	{
		//delete older than 1 second
		tp_changes.erase(tp_changes.begin(), tp_changes.lower_bound(new_tp - 1.0L));
	}
	if (velocity)
	{
		//apply some velocity here
		*velocity -= delta_time;
		if (velocity <= 0.0L)
		{
			velocity.reset();
		}
		else
		{
			_offset = std::clamp(_offset - *velocity * velmul * delta_time, min_offset, max_offset);
		}
	}
	return stared_scrolling;
	/*_offset = std::clamp(_offset, min_offset, max_offset); //fix?
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
				//_offset = get_offset();
				_offset += _fingers[finger_id].second;
				_offset = std::clamp(_offset, min_offset, max_offset);
				//_offset = get_offset();
				_fingers.erase(finger_id);
			}
		}
	}
	_offset = std::clamp(_offset, min_offset, max_offset); //fix?*/
}
