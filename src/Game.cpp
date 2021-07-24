#include "Game.h"

#include "Renderer.h"
#include "Font.h"
#include "StateDevMenu.h"
#include "DevTouch.h"
#include "Colors.h"
#include "BurstParticles.h"

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <curl/curl.h>

#include <algorithm>
#include <unordered_map>
#include <functional>

Game::Game()
{
	init_libraries();

	cfg = std::make_unique<Configuration>();
	cfg->load_from_file();

	renderer = std::make_unique<Renderer>(cfg->enable_vsync);
	audio = std::make_unique<AudioDevice>(cfg->audio_sample_rate, cfg->audio_stereo);

	DustMotes::enabled = cfg->enable_particles_dustmotes;
	BurstParticles::enabled = cfg->enable_particles_burst;

	change_state<StateDevMenu>();
}

Game::~Game()
{
	//make sure to destruct the old state
	stop();

	audio.reset();
	renderer.reset();
	cfg->save_to_file();

	deinit_libraries();
}

void Game::run()
{
	_state_changed = false;

	FpsCounter _fps_counter;
	DevTouch _dev_touch(renderer.get());
	Font fps_font(renderer.get(), "roboto.ttf", 4.0);

	//while there is an active state...
	while (_state)
	{
		//handle events
		_state->touch_up.clear();
		_state->touch_down.clear();
		_state->touch_move.clear();
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT) {
				return stop();
			}
			else if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
				renderer->reload();
			}
			else if (event.type == SDL_FINGERDOWN || event.type == SDL_FINGERUP || event.type == SDL_FINGERMOTION) {
				Vec2 position{ std::clamp(Number(event.tfinger.x) * 2.0L - 1.0L, -1.0L, 1.0L),
					std::clamp(Number(event.tfinger.y) * 2.0L - 1.0L, -1.0L, 1.0L) };
				std::unordered_map<uint32_t, std::reference_wrapper<std::map<uint16_t, Vec2>>>
				{   { SDL_FINGERDOWN,   _state->touch_down },
					{ SDL_FINGERUP,     _state->touch_up   },
					{ SDL_FINGERMOTION, _state->touch_move }
				}.at(event.type).get().emplace(event.tfinger.fingerId, position);
			}
		}

		if (cfg->enable_hit_markers) { _dev_touch.update(_state.get()); }

		//update the state
		_state->update();

		if (_state_changed) {
			_state_changed = false; }

		//if the state hasn't changed, try to render it
		else if (renderer->active()) {
			renderer->clear();
			_state->render();
			if (cfg->show_fps)
			{
				auto fps = _fps_counter.update();
				auto fps_font_color = Colors::GREEN;
				if (fps < 30)
					fps_font_color = Colors::RED;
				else if (fps < 60)
					fps_font_color = Colors::YELLOW;
				Texture fps_texture(renderer.get(), &fps_font, "FPS: " + std::to_string(fps), fps_font_color);
				renderer->render(&fps_texture, { 0,0 }, fps_texture.get_psize(), { -1,-1 }, fps_texture.get_rsize(), {}, { -1, -1 });
			}
			if (cfg->enable_hit_markers) { _dev_touch.render(renderer.get()); }
			renderer->display(); }

		//check for SDL errors
		std::string sdlerror(SDL_GetError());
		if (!sdlerror.empty()) {
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "United Tiles - SDL Error", sdlerror.c_str(), nullptr);
			SDL_ClearError(); }
	}
}

void Game::stop()
{
	_state_changed = true;
	_state.reset();
}

void Game::append_cfg()
{
	renderer->vsync = cfg->enable_vsync;
	renderer->reload();
	audio = std::make_unique<AudioDevice>(cfg->audio_sample_rate, cfg->audio_stereo);
	DustMotes::enabled = cfg->enable_particles_dustmotes;
	BurstParticles::enabled = cfg->enable_particles_burst;
	cfg->save_to_file();
}

void Game::init_libraries()
{
	SDL_SetHint(SDL_HINT_MOUSE_TOUCH_EVENTS, "1");
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");
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
