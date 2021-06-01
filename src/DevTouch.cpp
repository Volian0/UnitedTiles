#include "DevTouch.h"

#include <algorithm>

bool DevTouchMarker::should_die(const Timepoint& tp) const
{
	return tp - timepoint > std::chrono::seconds(2);
}

void DevTouchMarker::render(const DevTouch* dt, Renderer* renderer) const
{
	Number time = std::chrono::duration<Number>(dt->tp_now - timepoint).count();
	uint8_t tint = std::clamp<uint8_t>(std::clamp((time/2) * 255.0L, 0.0L, 255.0L), 0, 255);
	dt->texture.tint = { 255, tint, tint, 255 };
	renderer->render(&dt->texture, {}, dt->texture.get_psize(), position, {0.25, renderer->get_aspect_ratio() * 0.25}, position, {}, 45);
	renderer->render(finger_id.get(), {}, finger_id->get_psize(), position, finger_id->get_rsize(), {}, {}, 0);
}

DevTouch::DevTouch(Renderer* renderer)
: font{renderer, "roboto.ttf", 8 },
texture{renderer, "hit_marker.png"}
{
	texture.blend_mode = 1;
}

void DevTouch::update(GameState* state)
{
	tp_now = Clock::now();

	markers.erase(std::remove_if(markers.begin(), markers.end(), [&](const DevTouchMarker& marker) { return marker.should_die(tp_now); }), markers.end());

	for (const auto& td : state->touch_down)
	{
		markers.emplace_back(DevTouchMarker{ td.second, std::make_unique<Texture>(state->game->renderer.get(), &font, std::to_string(td.first), glm::u8vec4{ 255,255,255,255 }), tp_now });
	}
}

void DevTouch::render(Renderer* renderer) const
{
	for (const auto& marker : markers)
	{
		marker.render(this, renderer);
	}
}
