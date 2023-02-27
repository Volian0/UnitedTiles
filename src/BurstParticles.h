#pragma once

#include "NonCopyable.h"
#include "Types.h"
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
	Color color;
	Number lifetime;
	Number total_time = 0;
	std::vector<BurstParticle> _particles;
	[[nodiscard]] bool is_dead() const;
	[[nodiscard]] uint8_t get_alpha() const;
	void simulate(Number delta_time, Number aspect_ratio);;
};

class BurstParticles : NonCopyable
{
public:
	BurstParticles(Renderer* renderer, const std::string& texture_name);

	void add(Vec2 pos, Vec2 size, uint16_t number_of_particles, Number max_lifetime, Vec2 min_velocity, Vec2 max_velocity, Color color);

	void update(Number delta_time);
	void render() const;

	static bool enabled;

	mutable Texture _texture;

private:
	Renderer* _renderer;

	std::vector<BurstParticleGroup> _particle_groups;
};