#include "Texture.h"
#include "Font.h"
#include "Renderer.h"
#include "Path.h"

#include <SDL_image.h>
#include <SDL_ttf.h>

#include <chrono>
#include <iostream>
#include <thread>

Texture::Texture(Renderer* renderer, const std::string& filename)
	:RendererReloadable(renderer),
	type{ Type::FILE },
	info{ FileInfo{ filename } }
{

	while (true)
	{
		_ptr = IMG_LoadTexture(reinterpret_cast<SDL_Renderer*>(_renderer->_ptr), Path::res(filename, "textures").c_str());
		if (_ptr != nullptr)
		{
			break;			
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(250));
	}
	SDL_SetTextureScaleMode(reinterpret_cast<SDL_Texture*>(_ptr), SDL_ScaleModeBest);
	update_size();
	blend_mode = SDL_BLENDMODE_NONE;
}

void Texture::set_to_nearest()
{
	SDL_SetTextureScaleMode(reinterpret_cast<SDL_Texture*>(_ptr), SDL_ScaleModeNearest);
	m_nearest = true;
}

Texture::Texture(Renderer* renderer, const Font* font, const std::string& text2, Color color)
	:RendererReloadable(renderer),
	type{ Type::FONT },
	info{ FontInfo{ font->filename, font->size, text2.empty() ? " " : text2, color } }
{

	std::string text = text2.empty() ? " " : text2;
	SDL_Surface* surface = nullptr;
	while (true)
	{
		surface = TTF_RenderUTF8_Blended(reinterpret_cast<TTF_Font*>(font->_ptr), text.c_str(), { color.r,color.g,color.b,color.a });
		if (surface != nullptr)
		{
			break;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(250));
	}
while (true)
	{
		_ptr = SDL_CreateTextureFromSurface(reinterpret_cast<SDL_Renderer*>(_renderer->_ptr), surface);
		if (_ptr != nullptr)
		{
			break;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(250));
	}
	SDL_FreeSurface(surface);
	update_size();
	blend_mode = SDL_BLENDMODE_BLEND;
}

Texture::~Texture()
{
	unload();
	_ptr = nullptr;
}

void Texture::unload()
{
	SDL_DestroyTexture(reinterpret_cast<SDL_Texture*>(_ptr));
	_ptr = nullptr;
}

void Texture::reload()
{
	_ptr = nullptr;
	if (type == Type::FILE)
	{
		const FileInfo& file_info = std::get<FileInfo>(info);
			while (true)
	{
		_ptr = IMG_LoadTexture(reinterpret_cast<SDL_Renderer*>(_renderer->_ptr), Path::res(file_info.filename, "textures").c_str());
		if (_ptr != nullptr)
		{
			break;			
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(250));
	}
		SDL_SetTextureScaleMode(reinterpret_cast<SDL_Texture*>(_ptr), SDL_ScaleModeBest);
	}
	else if (type == Type::FONT)
	{
		const FontInfo& font_info = std::get<FontInfo>(info);
		Font font(_renderer, font_info.filename, font_info.size);
		SDL_Surface* surface = nullptr;

	while (true)
	{
		surface = TTF_RenderUTF8_Blended(reinterpret_cast<TTF_Font*>(font._ptr), font_info.text.c_str(), { font_info.color.r,font_info.color.g,font_info.color.b,font_info.color.a });
		if (surface != nullptr)
		{
			break;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(250));
	}
while (true)
	{
		_ptr = SDL_CreateTextureFromSurface(reinterpret_cast<SDL_Renderer*>(_renderer->_ptr), surface);
		if (_ptr != nullptr)
		{
			break;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(250));
	}
		SDL_FreeSurface(surface);
	}
	update_size();
	if (m_nearest)
	{
		set_to_nearest();
	}
}

glm::u32vec2 Texture::get_psize() const
{
	return _psize;
}

Vec2 Texture::get_rsize() const
{
	return _rsize;
}

void Texture::update_size()
{
	//std::cout << "updating size" << std::endl;
	int w, h;
	SDL_QueryTexture(reinterpret_cast<SDL_Texture*>(_ptr), nullptr, nullptr, &w, &h);
	_psize = { w, h };
	if (std::holds_alternative<FontInfo>(info))
	{
		_rsize = Vec2(_psize) * 0.0015L;
		_rsize.y *= _renderer->get_aspect_ratio();
	}
	else _rsize = Vec2(_psize) / Vec2(_renderer->get_size());
}