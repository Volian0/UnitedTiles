#include "Game.h"

#include "Renderer.h"
#include "StateTest.h"

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <curl/curl.h>

Game::Game()
{
	SDL_SetHint(SDL_HINT_MOUSE_TOUCH_EVENTS, "1");
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");
	init_libraries();

	renderer = std::make_unique<Renderer>();

	change_state<StateTest>();
}

Game::~Game()
{
	//make sure to destruct the old state
	stop();

	deinit_libraries();
}

void Game::run()
{
	_state_changed = false;

	//while there is an active state...
	while (_state)
	{
		//update the state
		_state->update();

		if (_state_changed)
		{
			_state_changed = false;
		}

		//if the state hasn't changed, try to render it
		else if (renderer->active())
		{
			renderer->clear();
			_state->render();
			renderer->display();
		}
	}
}

void Game::stop()
{
	_state_changed = true;
	_state.reset();
}

void Game::init_libraries()
{
	SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(IMG_INIT_PNG);
	TTF_Init();
	curl_global_init(CURL_GLOBAL_ALL);
}

void Game::deinit_libraries()
{
	curl_global_cleanup();
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}
