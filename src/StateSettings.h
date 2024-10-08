#pragma once

#include "GameState.h"
#include "Font.h"
#include "Texture.h"
#include "DevButton.h"

#include "ui/CheckBox.h"
#include "ui/Label.h"
#include "ui/ScrollablePanel.h"

#include <memory>

class StateSettings : public GameState
{
public:
	StateSettings(Game* game_);

private:
	virtual void update();
	virtual void render() const;

	Font _font;
	mutable Texture _bg;
	Texture _dev_button_texture;
	DevButton _b_discard;
	DevButton _b_apply;
	DevButton _b_soundfonts;
	DevButton _b_starting_tps;
	DevButton _b_rate_app;

	Texture cb_u, cb_c;
	Label ad_hint;

	ScrollablePanel scrollable_panel;

	std::vector<std::pair<CheckBox, std::reference_wrapper<bool>>> check_boxes;
};