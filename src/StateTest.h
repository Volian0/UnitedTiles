#pragma once

#include "GameState.h"

#include "Font.h"
#include "Texture.h"
#include "FpsCounter.h"

#include <memory>

class StateTest : public GameState
{
public:
	StateTest(Game* game_);

private:
	virtual void update();
	virtual void render() const;

	std::unique_ptr<Font> _font;
	std::unique_ptr<Texture> _bg;
	std::unique_ptr<Texture> _text;
	FpsCounter _fps_counter;
};
