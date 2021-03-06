#pragma once

#include "NonCopyable.h"

class Renderer;

class RendererReloadable : NonCopyable
{
public:
	RendererReloadable(Renderer* renderer);
	virtual ~RendererReloadable();

	virtual void unload() = 0;
	virtual void reload() = 0;

protected:
	Renderer* _renderer;
};