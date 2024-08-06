#include "Renderer.h"

#include "Texture.h"

#include <SDL.h>

#include <atomic>
#include <iostream>
#include <chrono>
#include <thread>

Renderer::Renderer(bool vsync_)
	:vsync{ vsync_ }
{
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
#ifdef __ANDROID__
	_window = SDL_CreateWindow("United Tiles", 0, 0, 0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_BORDERLESS | SDL_WINDOW_OPENGL | SDL_WINDOW_MAXIMIZED);
#else
SDL_DisplayMode DM;
SDL_GetCurrentDisplayMode(0, &DM);
auto height = DM.h;
decltype(height) width = static_cast<long double>(height) * (9.0L / 16.0L);
	_window = SDL_CreateWindow("United Tiles", (DM.w - width)/2, 0, width, height, SDL_WINDOW_BORDERLESS | SDL_WINDOW_ALWAYS_ON_TOP | SDL_WINDOW_OPENGL);
#endif
	_ptr = SDL_CreateRenderer(reinterpret_cast<SDL_Window*>(_window), -1, vsync ? SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC : SDL_RENDERER_ACCELERATED);
	update_size();
}

Renderer::~Renderer()
{
	SDL_DestroyRenderer(reinterpret_cast<SDL_Renderer*>(_ptr));
	SDL_DestroyWindow(reinterpret_cast<SDL_Window*>(_window));
}

void Renderer::clear()
{
	SDL_RenderClear(reinterpret_cast<SDL_Renderer*>(_ptr));
}

void Renderer::display()
{
	SDL_RenderPresent(reinterpret_cast<SDL_Renderer*>(_ptr));
}

void Renderer::minimize()
{
	auto window = reinterpret_cast<SDL_Window*>(_window);
	SDL_MinimizeWindow(window);
}

void Renderer::reload(bool recreate)
{
	std::cout << "Reloading renderer" << std::endl;
	std::unordered_set<RendererReloadable*> reloadables2 = _reloadables;
	if (recreate)
	{
		for (RendererReloadable* reloadable : reloadables2)
		{
			reloadable->unload();
		}
		SDL_DestroyRenderer(reinterpret_cast<SDL_Renderer*>(_ptr));
		_ptr = nullptr;
		while (true)
		{
			_ptr = SDL_CreateRenderer(reinterpret_cast<SDL_Window*>(_window), -1, vsync ? SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC : SDL_RENDERER_ACCELERATED);
			if (_ptr != nullptr)
			{
				break;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(250));
		}
	}
	update_size();
	if (recreate)
	{
		for (RendererReloadable* reloadable : reloadables2)
		{
			reloadable->reload();
		}
	}
	for (RendererReloadable* reloadable : reloadables2)
	{
		reloadable->update_size();
	}

	if (_clip_size != Vec2{})
	{
		set_clip_rect(_clip_position, _clip_size, _clip_origin);
	}
}

void Renderer::render(Texture* texture, glm::u32vec2 src_pos, glm::u32vec2 src_size, Vec2 dest_pos, Vec2 dest_size, Vec2 rot_origin, Vec2 src_origin, Number angle, uint8_t flip)
{
	if (_widescreen)
	{
		//dest_size.x /= _aspect_rato;
		//dest_pos.x /= _aspect_rato;
		//rot_origin.x /= _aspect_rato;
	}
	dest_size *= Vec2(_size);
	dest_pos = (dest_pos + Number(1.0L)) * Vec2(_size) / Number(2.0L) - (src_origin + Number(1.0L)) * Vec2(dest_size) / Number(2.0L);
	rot_origin = (rot_origin + Number(1.0L)) * Vec2(_size) / Number(2.0L) - dest_pos;

	SDL_Rect src_rect{ src_pos.x, src_pos.y, src_size.x, src_size.y };
	SDL_FPoint center{ rot_origin.x, rot_origin.y };
	SDL_FRect dest_rect{ dest_pos.x, dest_pos.y, dest_size.x, dest_size.y };

	SDL_SetTextureBlendMode(reinterpret_cast<SDL_Texture*>(texture->_ptr), static_cast<SDL_BlendMode>(texture->blend_mode));
	SDL_SetTextureColorMod(reinterpret_cast<SDL_Texture*>(texture->_ptr), texture->tint.r, texture->tint.g, texture->tint.b);
	SDL_SetTextureAlphaMod(reinterpret_cast<SDL_Texture*>(texture->_ptr), texture->tint.a);

	SDL_RenderCopyExF(reinterpret_cast<SDL_Renderer*>(_ptr), reinterpret_cast<SDL_Texture*>(texture->_ptr), &src_rect, &dest_rect, angle, &center, SDL_RendererFlip(flip));
}

glm::u16vec2 Renderer::get_size() const
{
	return _widescreen ? glm::u16vec2{_size.y, _size.y} : _size;
}

extern std::atomic_bool active_audio;

bool Renderer::active() const
{
	#ifdef __ANDROID__
	return true;
	#else
	bool active = !(SDL_GetWindowFlags(reinterpret_cast<SDL_Window*>(_window)) & SDL_WINDOW_MINIMIZED);
	active_audio = active;
	return active;
	#endif
}

void Renderer::set_clip_rect(Vec2 pos, Vec2 size, Vec2 origin)
{
	_clip_origin = origin;
	_clip_position = pos;
	_clip_size = size;

	size *= Vec2(_size);
	pos = (pos + Number(1.0L)) * Vec2(_size) / Number(2.0L) - (origin + Number(1.0L)) * Vec2(size) / Number(2.0L);
	/*std::cout << _clip_position.x << ", " << _clip_position.y << std::endl;
	std::cout << _clip_size.x << ", " << _clip_size.y << std::endl;
	std::cout << _clip_origin.x << ", " << _clip_origin.y << std::endl;
	std::cout << pos.x << ", " << pos.y << std::endl;
	std::cout << size.x << ", " << size.y << std::endl; std::abort();*/
	SDL_Rect dest_rect{ std::round(pos.x), std::round(pos.y), std::round(size.x), std::round(size.y) };

	SDL_RenderSetClipRect(reinterpret_cast<SDL_Renderer*>(_ptr), &dest_rect);
}

void Renderer::unset_clip()
{
	_clip_size = {};

	SDL_RenderSetClipRect(reinterpret_cast<SDL_Renderer*>(_ptr), nullptr);
}

void Renderer::update_size()
{
	int w, h;
	SDL_GetRendererOutputSize(reinterpret_cast<SDL_Renderer*>(_ptr), &w, &h);
	_size = { w, h };
	_aspect_rato = Number(w) / Number(h);
	_widescreen = w > h;
}