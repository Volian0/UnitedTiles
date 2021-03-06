#pragma once

#include "Types.h"
#include "Timepoint.h"
#include "Font.h"
#include "Renderer.h"
#include "Texture.h"
#include "Game.h"

#include <memory>
#include <vector>

struct DevTouchMarker
{
	Vec2 position;
	std::unique_ptr<Texture> finger_id;
	Timepoint timepoint;
	[[nodiscard]] bool should_die(const Timepoint& tp) const;
	void render(const class DevTouch* dt, Renderer* renderer) const;
};

class DevTouch : NonCopyable
{
	friend DevTouchMarker;

public:
	DevTouch(Renderer* renderer);

	void update(GameState* state);
	void render(Renderer* renderer) const;

private:
	Font font;
	mutable Texture texture;
	Timepoint tp_now;
	std::vector<DevTouchMarker> markers;
};