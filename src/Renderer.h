#pragma once

#include "NonCopyable.h"
#include "Types.h"

#include <memory>
#include <unordered_set>

class RendererReloadable;
class Texture;

class Renderer : NonCopyable
{
	friend RendererReloadable;
	friend Texture;

public:
	Renderer(bool vsync_ = false);
	~Renderer();

	void clear();
	void display();
	void reload(bool recreate);

	void minimize();

	void render(Texture* texture, glm::u32vec2 src_pos, glm::u32vec2 src_size,
		Vec2 dest_pos, Vec2 dest_size,
		Vec2 rot_origin,
		Vec2 src_origin = { 0.0L, 0.0L }, Number angle = 0.0L, uint8_t flip = 0);

	[[nodiscard]] glm::u16vec2 get_size() const;
	[[nodiscard]] constexpr Number get_aspect_ratio() const noexcept
	{
		return _widescreen ? 1.0L : _aspect_rato;
	}
	[[nodiscard]] bool active() const;
	bool vsync;

	void set_clip_rect(Vec2 pos, Vec2 size, Vec2 origin);
	void unset_clip();
private:
	Vec2 _clip_size{};
	Vec2 _clip_position{};
	Vec2 _clip_origin{};

	void* _ptr;
	void* _window;
	std::unordered_set<RendererReloadable*> _reloadables;
	glm::u16vec2 _size;
	Number _aspect_rato;
	bool _widescreen;
	void update_size();
};