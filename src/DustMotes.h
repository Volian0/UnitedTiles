#pragma once

#include "NonCopyable.h"
#include "Types.h"
#include "Renderer.h"
#include "Texture.h"
#include "Timepoint.h"

#include <vector>

struct DustMote
{
	Number total_time;
	Vec2 position;
	Number angular_direction;
	Number angular_velocity;
	Number size;
	Number lifetime;
	Number velocity;
	Number acceleration;
	[[nodiscard]] bool is_dead() const;
	[[nodiscard]] uint8_t get_alpha() const;
	void simulate(Number delta_time, Number aspect_ratio);
};

class DustMotes : NonCopyable
{
public:
	DustMotes(Color color, Renderer* renderer, const std::string& texture_name, Number min_size, Number max_size, Number particles_per_second, Number min_lifetime, Number max_lifetime, Number max_velocity, uint8_t blend_mode = 1);

	void update(Number delta_time);
	void render() const;

	bool dont_spawn=false;

	static bool enabled;

private:

	Color _color;
	Renderer* _renderer;
	mutable Texture _texture;
	Number _min_size;
	Number _max_size;
	Number _particles_per_second;
	Number _min_lifetime;
	Number _max_lifetime;
	Number _max_velocity;
	Number _accumulated_time;

	std::vector<DustMote> _dustmotes;
};