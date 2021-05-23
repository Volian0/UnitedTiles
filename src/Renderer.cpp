#include "Renderer.h"

#include "Texture.h"

#include <SDL/SDL.h>

#include <atomic>

Renderer::Renderer(bool vsync_)
	:vsync{ vsync_ }
{
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
#ifdef __ANDROID__
	_window = SDL_CreateWindow("United Tiles", 0, 0, 0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP);
#else
	_window = SDL_CreateWindow("United Tiles", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 360, 640, SDL_WINDOW_RESIZABLE);
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

void Renderer::reload()
{
	for (RendererReloadable* reloadable : _reloadables)
	{
		reloadable->unload();
	}
	SDL_DestroyRenderer(reinterpret_cast<SDL_Renderer*>(_ptr));
	_ptr = SDL_CreateRenderer(reinterpret_cast<SDL_Window*>(_window), -1, vsync ? SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC : SDL_RENDERER_ACCELERATED);
	update_size();
	for (RendererReloadable* reloadable : _reloadables)
	{
		reloadable->reload();
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

Number Renderer::get_aspect_ratio() const
{
	return _widescreen ? 1.0L : _aspect_rato;
}

extern std::atomic_bool active_audio;

bool Renderer::active() const
{
	bool active = !(SDL_GetWindowFlags(reinterpret_cast<SDL_Window*>(_window)) & SDL_WINDOW_MINIMIZED);
	active_audio = active;
	return active;
}

void Renderer::update_size()
{
	int w, h;
	SDL_GetRendererOutputSize(reinterpret_cast<SDL_Renderer*>(_ptr), &w, &h);
	_size = { w, h };
	_aspect_rato = Number(w) / Number(h);
	_widescreen = w > h;
}