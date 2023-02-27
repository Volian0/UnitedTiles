#pragma once

#include "../Types.h"
#include "../NonCopyable.h"

#include <map>
#include <iostream>
#include <optional>

class ScrollablePanel : NonCopyable
{
public:
	Vec2 position;
	Vec2 size;
	Vec2 origin = { 0.0L, 0.0L };

	Number min_offset = 0.0L;
	Number max_offset = 1.0L;

	void start_rendering() const;
	void stop_rendering() const;

	[[nodiscard]] constexpr bool in_range(Vec2 pos) const noexcept
	{
		Vec2 scaled_size = size * 2.0L;
		Vec2 dest_pos = position - (origin / 2.0L + 0.5L) * scaled_size;
		//Vec2 dest_pos = (position + Number(1.0L)) * Vec2(2.0L) / Number(2.0L) - (origin + Number(1.0L)) * Vec2(size) / Number(2.0L);
		/*std::cout << position.x << ", " << position.y << std::endl;
		std::cout << origin.x << ", " << origin.y << std::endl;
		std::cout << dest_pos.x << ", " << dest_pos.y << std::endl;
		std::abort();*/
		return (pos.x >= dest_pos.x && pos.x <= dest_pos.x + scaled_size.x) && (pos.y >= dest_pos.y && pos.y <= dest_pos.y + scaled_size.y);
	}

	[[nodiscard]] Number get_offset() const;

	bool update(Number t_aspect_ratio);

	class GameState* _state;

	constexpr auto reset() -> void
	{
		_last_finger_position.reset();
		last_y_pos.reset();
		_offset = 0;
	}

	constexpr auto is_scrolled() -> bool
	{
		return !static_cast<bool>(last_y_pos) && static_cast<bool>(_last_finger_position);
	}

	std::optional<Number> last_y_pos;

private:
	Number _offset = 0;

	std::optional<std::pair<FingerID, Number>> _last_finger_position;
	//std::map<FingerID, std::pair<Number, Number>> _fingers;
};