#pragma once

#include "Configuration.h"
#include "GameState.h"
#include "FpsCounter.h"
#include "AudioDevice.h"
#include "AdManager.hpp"
#include "Timepoint.h"
#include "Music.hpp"

#include <memory>
#include <string_view>

class Renderer;

class Game : NonCopyable
{
public:
	Game(std::string_view t_song_filename = {});
	~Game();

	void run();
	void stop();

	void append_cfg();

	AdManager ad_manager;

	template<typename T, typename... Args>
	void change_state(Args&&... args)
	{
		//set the flag to prevent the old state from rendering
		_state_changed = true;

		//destruct the old state
		_state.reset();

		//create the new state
		_state = std::make_unique<T>(this, std::forward<Args>(args)...);
	}

	std::unique_ptr<Configuration> cfg;

	std::unique_ptr<Renderer> renderer;
	std::unique_ptr<AudioDevice> audio;
	std::unique_ptr<Music> main_menu_music;

	void play_music();
	void stop_music();

private:
	bool m_music_playing = false;
	void init_libraries();
	void deinit_libraries();
	
	std::unique_ptr<GameState> _state;
	bool _state_changed;
	bool _in_background = false;
	bool _in_foreground = true;
};