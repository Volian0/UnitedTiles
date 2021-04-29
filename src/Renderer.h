#pragma once

#include "Unique.h"
#include "Number.h"

#include <memory>
#include <unordered_set>

class RendererReloadable;
class Texture;

class Renderer : Unique
{
	friend RendererReloadable;
	friend Texture;

public:
	Renderer();
	~Renderer();

	void clear();
	void display();
	void reload();

	void render(Texture* texture, glm::u32vec2 src_pos, glm::u32vec2 src_size,
		Vec2 dest_pos, Vec2 dest_size,
		Vec2 rot_origin,
		Vec2 src_origin = { 0.0L, 0.0L }, Number angle = 0.0L);

	glm::u16vec2 get_size() const;
	bool active() const;

private:
	void* _ptr;
	void* _window;
	std::unordered_set<RendererReloadable*> _reloadables;
	glm::u16vec2 _size;
	void update_size();
};