#include "Renderer.h"

#include "Texture.h"

#include <SDL/SDL.h>

Renderer::Renderer()
{
#ifdef __ANDROID__
	_window = SDL_CreateWindow("United Tiles", 0, 0, 0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP);
#else
	_window = SDL_CreateWindow("United Tiles", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_RESIZABLE);
#endif
	_ptr = SDL_CreateRenderer(reinterpret_cast<SDL_Window*>(_window), -1, SDL_RENDERER_ACCELERATED);
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
	SDL_DestroyRenderer(reinterpret_cast<SDL_Renderer*>(_ptr));
	_ptr = SDL_CreateRenderer(reinterpret_cast<SDL_Window*>(_window), -1, SDL_RENDERER_ACCELERATED);
	update_size();
	for (RendererReloadable* reloadable : _reloadables)
	{
		reloadable->reload();
	}
}

void Renderer::render(Texture* texture, glm::u32vec2 src_pos, glm::u32vec2 src_size, Vec2 dest_pos, Vec2 dest_size, Vec2 rot_origin, Vec2 src_origin, Number angle)
{
	dest_size *= Vec2(_size);
	dest_pos = (dest_pos + 1.0L) * Vec2(_size) / 2.0L - (src_origin + 1.0L) * Vec2(dest_size) / 2.0L;
	rot_origin = (rot_origin + 1.0L) * Vec2(_size) / 2.0L - dest_pos;

	SDL_Rect src_rect{ src_pos.x, src_pos.y, src_size.x, src_size.y };
	SDL_FPoint center{ rot_origin.x, rot_origin.y };
	SDL_FRect dest_rect{ dest_pos.x, dest_pos.y, dest_size.x, dest_size.y };
	SDL_RenderCopyExF(reinterpret_cast<SDL_Renderer*>(_ptr), reinterpret_cast<SDL_Texture*>(texture->_ptr), &src_rect, &dest_rect, angle, &center, SDL_FLIP_NONE);
}

glm::u16vec2 Renderer::get_size() const
{
	return _size;
}

bool Renderer::active() const
{
	return !(SDL_GetWindowFlags(reinterpret_cast<SDL_Window*>(_window)) & SDL_WINDOW_MINIMIZED);
}

void Renderer::update_size()
{
	int w, h;
	SDL_GetRendererOutputSize(reinterpret_cast<SDL_Renderer*>(_ptr), &w, &h);
	_size = { w, h };
}
