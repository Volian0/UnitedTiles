#pragma once

#include "RendererReloadable.h"
#include "Types.h"

#include <string>

class Texture;

class Font : public RendererReloadable
{
	friend Texture;

public:
	Font(Renderer* renderer, const std::string& filename_, Number size_);
	~Font();

	const std::string filename;
	const Number size;

	[[nodiscard]] constexpr auto get_height() const noexcept -> Number
	{
		return _height;
	}

private:
	void* _ptr=nullptr;
	Number _height;

	void unload() override;
	void reload() override;
};