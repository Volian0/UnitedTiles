#include "Font.h"

#include "Renderer.h"
#include "Path.h"

#include <SDL_ttf.h>

#include <thread>
#include <chrono>
#include <iostream>

Font::Font(Renderer* renderer, const std::string& filename_, Number size_)
	:RendererReloadable(renderer),
	filename{ filename_ }, size{ size_ },
	_ptr{ nullptr }
{
	while (true)
	{
		_ptr = TTF_OpenFont(Path::res(filename, "fonts").c_str(), size * 7.0F /** Number(_renderer->get_size().x) * 0.01L)*/);
		if (_ptr != nullptr)
		{
			break;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(250));
	}
	_height = Number(TTF_FontLineSkip(reinterpret_cast<TTF_Font*>(_ptr))) / Number(_renderer->get_size().y);
}

Font::~Font()
{
	unload();
}

void Font::unload()
{
	TTF_CloseFont(reinterpret_cast<TTF_Font*>(_ptr));
	_ptr = nullptr;
}

void Font::reload()
{
	_ptr = nullptr;
	while (true)
	{
		_ptr = TTF_OpenFont(Path::res(filename, "fonts").c_str(), size * 7.0F);
		if (_ptr != nullptr)
		{
			break;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(250));
	}
	_height = Number(TTF_FontLineSkip(reinterpret_cast<TTF_Font*>(_ptr))) / Number(_renderer->get_size().y);
}
