#pragma once

#include "Unique.h"

class Game;

class GameState : Unique
{
public:
	virtual void update() = 0;
	virtual void render() const = 0;

	virtual ~GameState() = default;

protected:
	GameState(Game* game_);

	Game* game;
};