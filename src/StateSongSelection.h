#pragma once

#include "GameState.h"

#include <memory>

class StateSongSelection : public GameState
{
public:
	StateSongSelection(Game* game_);

private:
	virtual void update();
	virtual void render() const;
};

