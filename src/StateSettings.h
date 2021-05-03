#pragma once

#include "GameState.h"
#include "Font.h"
#include "Texture.h"
#include "DevButton.h"

#include <memory>

class StateSettings : public GameState
{
public:
	StateSettings(Game* game_);

private:
	virtual void update();
	virtual void render() const;

	std::unique_ptr<Font> _font;
	std::unique_ptr<Texture> _bg;
	std::unique_ptr<Texture> _dev_button_texture;
	std::vector<DevButton> _dev_buttons;
};