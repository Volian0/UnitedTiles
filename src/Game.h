#pragma once

#include "Configuration.h"
#include "GameState.h"
#include "FpsCounter.h"
#include "AudioDevice.h"

#include <memory>

class Renderer;

class Game : NonCopyable
{
public:
	Game();
	~Game();

	void run();
	void stop();

	void append_cfg();

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

private:
	void init_libraries();
	void deinit_libraries();
	
	std::unique_ptr<GameState> _state;
	bool _state_changed;
};