#pragma once

#include "Unique.h"
#include "Number.h"
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
	bool is_dead() const;
	uint8_t get_alpha() const;
	void simulate(Number delta_time, Number aspect_ratio);
};

class DustMotes : Unique
{
public:
	DustMotes(glm::u8vec4 color, Renderer* renderer, const std::string& texture_name, Number min_size, Number max_size, Number particles_per_second, Number min_lifetime, Number max_lifetime, Number max_velocity);

	void update(Number delta_time);
	void render() const;

private:

	glm::u8vec4 _color;
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