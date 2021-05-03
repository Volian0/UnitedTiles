#include "Font.h"

#include "Renderer.h"
#include "Path.h"

#include <SDL/SDL_ttf.h>

Font::Font(Renderer* renderer, const std::string& filename_, Number size_)
	:RendererReloadable(renderer),
	filename{ filename_ }, size{ size_ },
	_ptr{ TTF_OpenFont(Path::res(filename, "fonts").c_str(), size * Number(_renderer->get_size().x) * 0.01L) }
{
}

Font::~Font()
{
	unload();
}

void Font::unload()
{
	TTF_CloseFont(reinterpret_cast<TTF_Font*>(_ptr));
}

void Font::reload()
{
	_ptr = TTF_OpenFont(Path::res(filename, "fonts").c_str(), size * Number(_renderer->get_size().x) * 0.01L);
}
