#pragma once

#include "../Types.h"
#include "../NonCopyable.h"

#include <map>

class ScrollablePanel : NonCopyable
{
public:
	Vec2 position;
	Vec2 size;
	Vec2 origin = { 0.0L, 0.0L };

	Number min_offset = 0.0;
	Number max_offset = 1.0;

	void start_rendering() const;
	void stop_rendering() const;

	[[nodiscard]] inline bool in_range(Vec2 pos) const
	{
		Vec2 dest_pos = (position + Number(1.0L)) * Vec2(2.0L) / Number(2.0L) - (origin + Number(1.0L)) * Vec2(size) / Number(2.0L) - 1.0L;
		return (pos.x >= dest_pos.x && pos.x <= dest_pos.x + size.x) && (pos.y >= dest_pos.y && pos.y <= dest_pos.y + size.y);
	}

	[[nodiscard]] Number get_offset() const;

	void update();

	class GameState* _state;

private:
	Number _offset = 0;

	std::map<FingerID, std::pair<Number, Number>> _fingers;
};