#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio/miniaudio.h"

#include "fluidsynth/fluidsynth.h"

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>

#include <glm/glm.hpp>

#include <curl/curl.h>

#include <vector>
#include <memory>
#include <string>
#include <thread>
#include <chrono>

int fluid_synth_write_s16_interleaved_stereo(fluid_synth_t* synth, int len, void* out)
{
	return fluid_synth_write_s16(synth, len, out, 0, 2, out, 1, 2);
}

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
	fluid_synth_write_s16_interleaved_stereo((fluid_synth_t*)(pDevice->pUserData), frameCount, pOutput);
}

std::string temppath;

void extract_asset_to_temp_file(const std::string& asset_path)
{
	auto* ifile = SDL_RWFromFile(asset_path.c_str(), "r");
	auto* ofile = SDL_RWFromFile(temppath.c_str(), "w");
	auto size = SDL_RWsize(ifile);
	std::unique_ptr<char[]> buffer = std::make_unique<char[]>(size);
	SDL_RWread(ifile, buffer.get(), size, 1);
	SDL_RWwrite(ofile, buffer.get(), size, 1);
	SDL_RWclose(ifile);
	SDL_RWclose(ofile);
}

void play_some_notes(fluid_synth_t* synth)
{
	fluid_synth_noteon(synth, 0, 50, 127);
	fluid_synth_noteon(synth, 0, 54, 127);
	fluid_synth_noteon(synth, 0, 38, 127);
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	fluid_synth_all_notes_off(synth, 0);
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	fluid_synth_noteon(synth, 0, 51, 127);
	fluid_synth_noteon(synth, 0, 55, 127);
	fluid_synth_noteon(synth, 0, 39, 127);
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	fluid_synth_all_notes_off(synth, 0);
}

void play_audio()
{
	fluid_settings_t* settings;
	fluid_synth_t* synth;
	settings = new_fluid_settings();
	fluid_settings_setnum(settings, "synth.gain", 0.5);
	synth = new_fluid_synth(settings);
	extract_asset_to_temp_file("test.sf2");
	fluid_synth_sfload(synth, temppath.data(), 1);
	ma_device_config deviceConfig;
	ma_device device;
	deviceConfig = ma_device_config_init(ma_device_type_playback);
	deviceConfig.playback.format = ma_format_s16;
	deviceConfig.playback.channels = 2;
	deviceConfig.sampleRate = 44100;
	deviceConfig.dataCallback = data_callback;
	deviceConfig.pUserData = synth;
	ma_device_init(0, &deviceConfig, &device);
	ma_device_start(&device);
	fluid_synth_program_change(synth, 0, 0);

	fluid_synth_noteon(synth, 0, 40, 100);
	std::this_thread::sleep_for(std::chrono::milliseconds(300));
	fluid_synth_noteon(synth, 0, 43, 110);
	std::this_thread::sleep_for(std::chrono::milliseconds(250));
	fluid_synth_noteon(synth, 0, 47, 120);
	std::this_thread::sleep_for(std::chrono::milliseconds(900));
	fluid_synth_all_notes_off(synth, 0);
	fluid_synth_noteon(synth, 0, 35, 100);
	std::this_thread::sleep_for(std::chrono::milliseconds(300));
	fluid_synth_noteon(synth, 0, 37, 110);
	std::this_thread::sleep_for(std::chrono::milliseconds(250));
	fluid_synth_noteon(synth, 0, 40, 127);
	std::this_thread::sleep_for(std::chrono::seconds(10));
	ma_device_stop(&device);
}

void curl_test()
{
	CURL* curl;
	CURLcode res;

	curl = curl_easy_init();
	if (curl)
	{
		curl_easy_setopt(curl, CURLOPT_URL, "https://example.com");
		/* example.com is redirected, so we tell libcurl to follow redirection */
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

		/* Perform the request, res will get the return code */
		res = curl_easy_perform(curl);
		/* Check for errors */
		//SDL_ShowSimpleMessageBox(0, "w", std::to_string(res).data(), 0);
		if (res != CURLE_OK)
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
				curl_easy_strerror(res));

		/* always cleanup */
		curl_easy_cleanup(curl);
	}
}

int main(int, char**) 
{
	curl_test();

	glm::vec2 test_vector = {3, 4};

	SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(IMG_INIT_PNG);
	TTF_Init();

	SDL_SetHintWithPriority(SDL_HINT_RENDER_SCALE_QUALITY, "2", SDL_HINT_OVERRIDE);

	temppath = std::string(SDL_GetPrefPath("volian", "unitedtiles")) + "file.temp";

	SDL_Window* window = SDL_CreateWindow("Test",0,0,640,480,SDL_WINDOW_FULLSCREEN_DESKTOP);
	SDL_Renderer* renderer =  SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED);
	int w, h;
	SDL_GetRendererOutputSize(renderer, &w, &h);
	SDL_Texture* background = IMG_LoadTexture(renderer, "bg.png");
	TTF_Font* font = TTF_OpenFont("roboto.ttf", w/10);
	SDL_Surface* font_surface = TTF_RenderText_Blended(font, "United Tiles", SDL_Color{ 255,255,255,255 });
	SDL_Texture* font_texture = SDL_CreateTextureFromSurface(renderer, font_surface);
	SDL_Rect dest_rect{(w-font_surface->w)/2,(h-font_surface->h)/2,font_surface->w,font_surface->h};

	std::thread thread(play_audio);

	std::chrono::time_point<std::chrono::steady_clock> tp1 = std::chrono::steady_clock::now();
	while (true)
	{
		std::chrono::time_point<std::chrono::steady_clock> tp2 = std::chrono::steady_clock::now();
		double total_elapsed_time = std::chrono::duration<double>(tp2 - tp1).count();

		SDL_Event event;
		while (SDL_PollEvent(&event))
			if (event.type == SDL_QUIT)
				quick_exit(0);

		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, background, nullptr, nullptr);
		SDL_RenderCopyEx(renderer, font_texture, nullptr, &dest_rect, sin(total_elapsed_time * 2) * 10, nullptr, SDL_FLIP_NONE);
		SDL_RenderPresent(renderer);
	}
	return 0;
}
