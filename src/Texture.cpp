#include "Texture.h"
#include "Font.h"
#include "Renderer.h"
#include "Path.h"

#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>

Texture::Texture(Renderer* renderer, const std::string& filename)
	:RendererReloadable(renderer),
	type{ Type::FILE },
	info{ FileInfo{ filename } }
{
	_ptr = IMG_LoadTexture(reinterpret_cast<SDL_Renderer*>(_renderer->_ptr), Path::res(filename, "textures").c_str());
	SDL_SetTextureScaleMode(reinterpret_cast<SDL_Texture*>(_ptr), SDL_ScaleModeBest);
	update_size();
	blend_mode = SDL_BLENDMODE_NONE;
}

Texture::Texture(Renderer* renderer, Font* font, const std::string& text, Color color)
	:RendererReloadable(renderer),
	type{ Type::FONT },
	info{ FontInfo{ font->filename, font->size, text, color } }
{
	SDL_Surface* surface = TTF_RenderText_Blended(reinterpret_cast<TTF_Font*>(font->_ptr), text.c_str(), { color.r,color.g,color.b,color.a });
	_ptr = SDL_CreateTextureFromSurface(reinterpret_cast<SDL_Renderer*>(_renderer->_ptr), surface);
	SDL_FreeSurface(surface);
	update_size();
	blend_mode = SDL_BLENDMODE_BLEND;
}

Texture::~Texture()
{
	unload();
}

void Texture::unload()
{
	SDL_DestroyTexture(reinterpret_cast<SDL_Texture*>(_ptr));
}

void Texture::reload()
{
	if (type == Type::FILE)
	{
		const FileInfo& file_info = std::get<FileInfo>(info);
		_ptr = IMG_LoadTexture(reinterpret_cast<SDL_Renderer*>(_renderer->_ptr), Path::res(file_info.filename, "textures").c_str());
		SDL_SetTextureScaleMode(reinterpret_cast<SDL_Texture*>(_ptr), SDL_ScaleModeBest);
	}
	else if (type == Type::FONT)
	{
		const FontInfo& font_info = std::get<FontInfo>(info);
		Font font(_renderer, font_info.filename, font_info.size);
		SDL_Surface* surface = TTF_RenderText_Blended(reinterpret_cast<TTF_Font*>(font._ptr), font_info.text.c_str(), { font_info.color.r,font_info.color.g,font_info.color.b,font_info.color.a });
		_ptr = SDL_CreateTextureFromSurface(reinterpret_cast<SDL_Renderer*>(_renderer->_ptr), surface);
		SDL_FreeSurface(surface);
	}
	update_size();
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
	int w, h;
	SDL_QueryTexture(reinterpret_cast<SDL_Texture*>(_ptr), nullptr, nullptr, &w, &h);
	_psize = { w, h };
	_rsize = Vec2(_psize) / Vec2(_renderer->get_size());
}