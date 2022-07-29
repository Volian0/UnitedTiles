#pragma once

#include "GameState.h"

#include "Font.h"
#include "Texture.h"
#include "FpsCounter.h"
#include "DevButton.h"
#include "Timepoint.h"
#include "DustMotes.h"
#include "Soundfont.h"

#include <memory>
#include <vector>

class StateSplash : public GameState
{
public:
	StateSplash(Game* game_);

private:
	virtual void update();
	virtual void render() const;

	std::unique_ptr<Font> _font;
	std::unique_ptr<Texture> _text;

	std::optional<Timepoint> _first_tp;
	Timepoint _new_tp;
	Number _required_time = 2.0L;
};
