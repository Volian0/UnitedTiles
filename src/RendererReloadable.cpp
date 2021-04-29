#include "RendererReloadable.h"

#include "Renderer.h"

RendererReloadable::RendererReloadable(Renderer* renderer)
	:_renderer{ renderer}
{
	_renderer->_reloadables.emplace(this);
}

RendererReloadable::~RendererReloadable()
{
	_renderer->_reloadables.erase(this);
}
