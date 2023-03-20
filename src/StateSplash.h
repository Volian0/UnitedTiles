#pragma once

#include "GameState.h"

#include "Font.h"
#include "Texture.h"
#include "FpsCounter.h"
#include "DevButton.h"
#include "Timepoint.h"
#include "DustMotes.h"
#include "Soundfont.h"
#include "DevButton.h"

#include <memory>
#include <vector>

class StateSplash : public GameState
{
public:
	StateSplash(Game* game_);

private:
	virtual void update();
	virtual void render() const;

	mutable Texture m_logo;
	mutable Texture m_white;
	Font m_font;
	mutable Texture m_text;
	mutable std::optional<Timepoint> m_first_tp;
	
	DustMotes m_dustmotes;

	Number m_required_time = 3.0L;
	Number m_passed_time = 0.0L;
	//Number m_angle = 0.0L;
	std::optional<Timepoint> m_tp_now;
};
