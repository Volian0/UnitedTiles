#pragma once

#include "NonCopyable.h"
#include "Types.h"

#include <vector>
#include <unordered_map>
#include <map>
#include <string>

class Game;

struct TouchEvent
{
	enum class Type : uint8_t {DOWN, MOVE, UP} type;
	uint16_t finger_id;
	Vec2 position;
};

class GameState : NonCopyable
{
public:
	virtual void update() = 0;
	virtual void render() const = 0;

	virtual ~GameState() = default;

	std::vector<TouchEvent> touch_events;
	std::string key_events;
	bool pressed_backspace = false;
	bool pressed_enter = false;

	Game* game;

protected:
	GameState(Game* game_);
};