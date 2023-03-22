#include "Game.h"

#include "Renderer.h"
#include "Font.h"
#include "StateSplash.h"
#include "StateSongMenu.h"
#include "StateLevel.h"
#include "DevTouch.h"
#include "Colors.h"
#include "BurstParticles.h"
#include "RNG.h"
#include "NetworkFile.h"

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <curl/curl.h>

#include <algorithm>
#include <unordered_map>
#include <functional>
#include <thread>
#include <atomic>
#include <chrono>
#include <filesystem>
#include <iostream>

Game::Game(std::string_view t_song_filename)
{
	init_libraries();

	cfg = std::make_unique<Configuration>();
	cfg->load_from_file();

	renderer = std::make_unique<Renderer>(cfg->enable_vsync);
	audio = std::make_unique<AudioDevice>(cfg->audio_sample_rate, cfg->audio_stereo);

	DustMotes::enabled = cfg->enable_particles_dustmotes;
	BurstParticles::enabled = cfg->enable_particles_burst;

	if (!t_song_filename.empty())
	{
		const std::filesystem::path song_path{t_song_filename};
		std::cout << "Song path: " << song_path.string() << std::endl;
		if (std::filesystem::exists(song_path))
		{
			if (std::filesystem::is_regular_file(song_path))
			{
				if (song_path.extension().string() == ".usong")
				{
					change_state<StateLevel>(std::uint16_t{65535}, std::string_view{t_song_filename});
				}
				else
				{
					SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "United Tiles", "Provided song has invalid extension", nullptr);	
				}
			}
			else
			{
				SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "United Tiles", "Provided song is not a file", nullptr);
			}
		}
		else
		{
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "United Tiles", "Provided song doesn't exist", nullptr);
		}
	}
	else
	{
		change_state<StateSplash>();
	}

	//change_state<StateLevel>(0);
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

std::atomic_bool mutex_network_usage = false;

void Game::run()
{
	_state_changed = false;

	FpsCounter _fps_counter;
	DevTouch _dev_touch(renderer.get());
	Font fps_font(renderer.get(), "roboto.ttf", 4.0);

	//while there is an active state...
	while (_state)
	{
		while (mutex_network_usage)
		{
			std::this_thread::yield();
		}
		std::scoped_lock lock{NetworkFile::m_global_mutex};

		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_APP_WILLENTERBACKGROUND || event.type == SDL_APP_DIDENTERBACKGROUND) {
				_in_foreground = false;
				_in_background = true;
				std::cout << "Entering background..." << std::endl;
			}
			else if (event.type == SDL_APP_DIDENTERFOREGROUND)
			{
				_in_foreground = true;
			}
			else if (event.type == SDL_QUIT) {
				if (!_in_background)
					return stop();
			}
			else if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED
			|| event.type == SDL_RENDER_DEVICE_RESET || event.type == SDL_RENDER_TARGETS_RESET) {
				if (!_in_background)
					renderer->reload();
			}
			else if (event.type == SDL_FINGERDOWN || event.type == SDL_FINGERUP || event.type == SDL_FINGERMOTION) {
				const Vec2 position{ std::clamp(Number(event.tfinger.x) * 2.0L - 1.0L, -1.0L, 1.0L),
					std::clamp(Number(event.tfinger.y) * 2.0L - 1.0L, -1.0L, 1.0L) };

				_state->touch_events.emplace_back(TouchEvent{[&](){
					if (event.type == SDL_FINGERDOWN)
						return TouchEvent::Type::DOWN;
					if (event.type == SDL_FINGERMOTION)
						return TouchEvent::Type::MOVE;
					return TouchEvent::Type::UP;
				}(), event.tfinger.fingerId, position});
			}
			else if (event.type == SDL_TEXTINPUT)
			{
				_state->key_events = event.text.text;
			}
			else if (event.type == SDL_KEYDOWN)
			{
				const auto sym = event.key.keysym.sym;
				if (sym == SDLK_BACKSPACE)
				{
					_state->pressed_backspace = true;
				}
				else if (sym == SDLK_RETURN)
				{
					_state->pressed_enter = true;
				}
			}
		}

		if (_in_background)
		{
			if (_in_foreground)
			{
				std::cout << "Entering foreground..." << std::endl;
				_in_background = false;
				renderer->reload();
			}
			else
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
				continue;
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
				auto lfps = _fps_counter.get_low_fps();
				auto lfps_font_color = Colors::GREEN;
				if (lfps < 30)
					lfps_font_color = Colors::RED;
				else if (lfps < 60)
					lfps_font_color = Colors::YELLOW;
				Texture lfps_texture(renderer.get(), &fps_font, "Low FPS: " + std::to_string(lfps), lfps_font_color);
				renderer->render(&lfps_texture, { 0,0 }, lfps_texture.get_psize(), { -1,-1 + lfps_texture.get_rsize().y * 2.0L}, lfps_texture.get_rsize(), {}, { -1, -1 });
			}
			if (cfg->enable_hit_markers) { _dev_touch.render(renderer.get()); }
			renderer->display();
			if (cfg->fake_lag)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds{RNG::integer(100,250)});
			}	
		}

		_state->touch_events.clear();
		_state->key_events.clear();
		_state->pressed_backspace = false;
		_state->pressed_enter = false;

		//check for SDL errors
		std::string sdlerror(SDL_GetError());
		if (!sdlerror.empty()) {
			if (cfg->show_sdl_errors)
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
