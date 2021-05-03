#pragma once

#include "Unique.h"

class Renderer;

class RendererReloadable : Unique
{
public:
	RendererReloadable(Renderer* renderer);
	virtual ~RendererReloadable();

	virtual void unload() = 0;
	virtual void reload() = 0;

protected:
	Renderer* _renderer;
};