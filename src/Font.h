#pragma once

#include "RendererReloadable.h"
#include "Number.h"

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

private:
	void* _ptr;

	void unload() override;
	void reload() override;
};