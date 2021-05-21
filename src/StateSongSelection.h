#pragma once

#include "GameState.h"
#include "SongInfo.h"
#include "DevButton.h"

#include <memory>

class StateSongSelection : public GameState
{
public:
	StateSongSelection(Game* game_);

private:
	virtual void update();
	virtual void render() const;

	SongList song_list;

	std::unique_ptr<Font> _font;
	std::unique_ptr<Texture> _bg;
	std::unique_ptr<Texture> _dev_button_texture;

	std::unique_ptr<DevButton>    left_button;
	std::unique_ptr<DevButton>   right_button;
	std::unique_ptr<DevButton> confirm_button;
	std::unique_ptr<DevButton> return_button;

	static uint32_t song_index;
};

