#pragma once

#include "Unique.h"
#include "Number.h"
#include "Renderer.h"
#include "Texture.h"
#include "Timepoint.h"

#include <vector>

struct BurstParticle
{
	Vec2 position;
	Vec2 velocity;
	Number angle = 0;
	void simulate(Number delta_time, Number aspect_ratio);
};

struct BurstParticleGroup
{
	glm::u8vec4 color;
	Number lifetime;
	Number total_time = 0;
	std::vector<BurstParticle> _particles;
	bool is_dead() const;
	uint8_t get_alpha() const;
	void simulate(Number delta_time, Number aspect_ratio);;
};

class BurstParticles : Unique
{
public:
	BurstParticles(Renderer* renderer, const std::string& texture_name);

	void add(Vec2 pos, Vec2 size, uint16_t number_of_particles, Number max_lifetime, Vec2 min_velocity, Vec2 max_velocity, glm::u8vec4 color);

	void update(Number delta_time);
	void render() const;

private:
	Renderer* _renderer;
	mutable Texture _texture;

	std::vector<BurstParticleGroup> _particle_groups;
};