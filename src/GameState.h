#pragma once

#include "NonCopyable.h"
#include "Types.h"

#include <vector>
#include <unordered_map>
#include <map>

class Game;

class GameState : NonCopyable
{
public:
	virtual void update() = 0;
	virtual void render() const = 0;

	virtual ~GameState() = default;

	//std::map<uint16_t, Vec2> touch_held;
	std::map<uint16_t, Vec2> touch_move;
	std::map<uint16_t, Vec2> touch_up;
	std::map<uint16_t, Vec2> touch_down;

	Game* game;

protected:
	GameState(Game* game_);
};