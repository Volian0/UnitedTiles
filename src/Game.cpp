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

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <curl/curl.h>

#include <algorithm>
#include <unordered_map>
#include <functional>
#include <thread>
#include <atomic>
#include <chrono>
#include <filesystem>
#include <iostream>
#include <cstdlib>

Game::Game(std::string_view t_song_filename)
{
	init_libraries();

	cfg = std::make_unique<Configuration>();
	cfg->load_from_file();

	renderer = std::make_unique<Renderer>(cfg->enable_vsync);
	audio = std::make_unique<AudioDevice>(cfg->audio_sample_rate, cfg->audio_stereo);

		main_menu_music = std::make_unique<Music>(Path::res("menu.ogg", "sounds"));
			sfx_press = std::make_unique<Sound>(Path::res("press.wav", "sounds"));
	sfx_tick = std::make_unique<Sound>(Path::res("tick.wav", "sounds"));


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

	main_menu_music.reset();
	audio.reset();
	renderer.reset();
	cfg->save_to_file();

	deinit_libraries();
}

std::atomic_bool mutex_network_usage = false;

extern std::atomic_bool active_audio;


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
				active_audio = false;
			}
			else if (event.type == SDL_APP_DIDENTERFOREGROUND)
			{
				active_audio = true;
				_in_foreground = true;
			}
			else if (event.type == SDL_QUIT) {
				if (!_in_background)
					return stop();
			}
			else if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED
			|| event.type == SDL_RENDER_DEVICE_RESET || event.type == SDL_RENDER_TARGETS_RESET) {
				if (!_in_background)
					renderer->reload(cfg->reload_renderer);
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
			else if (event.type == SDL_KEYDOWN && !event.key.repeat)
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
				else if (sym == SDLK_ESCAPE)
				{
					std::exit(0);
				}
				else if (pc_controls)
				{
					if (sym == SDLK_q || sym == SDLK_d || sym == SDLK_s || sym == SDLK_z)
					{
						_state->touch_events.emplace_back(TouchEvent{
							TouchEvent::Type::DOWN, 666, Vec2{-0.75,0.0}});
					}
					else if (sym == SDLK_e || sym == SDLK_f|| sym == SDLK_g || sym == SDLK_c)
					{
						_state->touch_events.emplace_back(TouchEvent{
							TouchEvent::Type::DOWN, 667, Vec2{-0.25,0.0}});
					}
					else if (sym == SDLK_i || sym == SDLK_j|| sym == SDLK_h || sym == SDLK_b)
					{
						_state->touch_events.emplace_back(TouchEvent{
							TouchEvent::Type::DOWN, 668, Vec2{0.25,0.0}});
					}
					else if (sym == SDLK_p || sym == SDLK_k|| sym == SDLK_l || sym == SDLK_m)
					{
						_state->touch_events.emplace_back(TouchEvent{
							TouchEvent::Type::DOWN, 669, Vec2{0.75,0.0}});
					}
				}
			}
			else if (event.type == SDL_KEYUP && !event.key.repeat && pc_controls)
			{
				const auto sym = event.key.keysym.sym;
					if (sym == SDLK_q || sym == SDLK_a || sym == SDLK_s || sym == SDLK_z)
					{
						_state->touch_events.emplace_back(TouchEvent{
							TouchEvent::Type::UP, 666, Vec2{-0.75,0.250}});
					}
					else if (sym == SDLK_e || sym == SDLK_d|| sym == SDLK_f || sym == SDLK_c)
					{
						_state->touch_events.emplace_back(TouchEvent{
							TouchEvent::Type::UP, 667, Vec2{-0.25,0.250}});
					}
					else if (sym == SDLK_i || sym == SDLK_j|| sym == SDLK_h || sym == SDLK_b)
					{
						_state->touch_events.emplace_back(TouchEvent{
							TouchEvent::Type::UP, 668, Vec2{0.25,0.250}});
					}
					else if (sym == SDLK_p || sym == SDLK_l|| sym == SDLK_k || sym == SDLK_m)
					{
						_state->touch_events.emplace_back(TouchEvent{
							TouchEvent::Type::UP, 669, Vec2{0.75,0.250}});
					}
			}
		}

		if (_in_background)
		{
			if (_in_foreground)
			{
				std::cout << "Entering foreground..." << std::endl;
				_in_background = false;
				renderer->reload(cfg->reload_renderer);
			}
			else
			{
				//SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "United Tiles - SDL Error", "bg", nullptr);
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

void Game::play_music()
{
	return;


	
	if (!cfg->menu_music)
	{
		return;
	}
	if (m_music_playing)
	{
		return;
	}
	m_music_playing = true;
	main_menu_music->fade_in(1.0F);
}
void Game::stop_music()
{
	return;



	if (!m_music_playing)
	{
		return;
	}
	main_menu_music->fade_out(0.5F);
	m_music_playing = false;
}

void Game::append_cfg()
{
	renderer->vsync = cfg->enable_vsync;
	renderer->reload(cfg->reload_renderer);
	main_menu_music->fade_out(0.0F);
	main_menu_music.reset();
	audio.reset();
	audio = std::make_unique<AudioDevice>(cfg->audio_sample_rate, cfg->audio_stereo);
	main_menu_music = std::make_unique<Music>(Path::res("menu.ogg", "sounds"));
	sfx_press = std::make_unique<Sound>(Path::res("press.wav", "sounds"));
	sfx_tick = std::make_unique<Sound>(Path::res("tick.wav", "sounds"));
	m_music_playing = false;
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
