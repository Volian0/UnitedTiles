#include "DustMotes.h"

#include "RNG.h"

#include <algorithm>

bool DustMote::is_dead() const
{
	return total_time >= lifetime;
}

uint8_t DustMote::get_alpha() const
{
	return std::clamp((1.0L - abs(total_time / lifetime - 0.5L) * 2.0L) * 255.0L, 0.0L, 255.0L);
}

void DustMote::simulate(Number delta_time, Number aspect_ratio)
{
	total_time += delta_time;
	velocity += acceleration * delta_time;
	angular_direction += angular_velocity * delta_time;
	position.x += sin(angular_direction) * velocity * 2.0L * delta_time;
	position.y += cos(angular_direction) * velocity * 2.0L * delta_time * aspect_ratio;
}

DustMotes::DustMotes(Color color, Renderer* renderer, const std::string& texture_name, Number min_size, Number max_size, Number particles_per_second, Number min_lifetime, Number max_lifetime, Number max_velocity, uint8_t blend_mode)
	:_color{color},
	_renderer{renderer},
	_texture{renderer, texture_name},
	_min_size{min_size},
	_max_size{max_size},
	_particles_per_second{particles_per_second},
	_min_lifetime{min_lifetime},
	_max_lifetime{max_lifetime},
	_max_velocity{max_velocity},
	_accumulated_time{0}
{
	_texture.blend_mode = blend_mode;
}

void DustMotes::update(Number delta_time)
{
	_accumulated_time += delta_time;

	//remove old particles
	_dustmotes.erase(std::remove_if(_dustmotes.begin(), _dustmotes.end(), [&](const DustMote& dustmote) { return dustmote.is_dead(); }), _dustmotes.end());

	//simulate existing particles
	for (DustMote& dustmote : _dustmotes)
	{
		dustmote.simulate(delta_time, _renderer->get_aspect_ratio());
	}

	//create new particles
	Number time_offset = 0.0L;
	for (Number spawn_rate = 1.0L / _particles_per_second; _accumulated_time > spawn_rate; _accumulated_time -= spawn_rate)
	{
		_dustmotes.emplace_back(DustMote{
			time_offset,								//total time
			Vec2(RNG::number(-1,1),RNG::number(-1,1)),	//position
			RNG::number(0,6.2831853L),					//angle
			RNG::number(0,1.0L),						//angular velocity
			RNG::number(_min_size,_max_size),			//size
			RNG::number(_min_lifetime,_max_lifetime),	//lifetime
			RNG::number(0, _max_velocity),				//speed
			RNG::number(-0.25L, 0.25L)					//acceleration
			});
		time_offset += spawn_rate;
	}
}

void DustMotes::render() const
{
	for (const DustMote& dustmote : _dustmotes)
	{
		_texture.tint = { _color.r, _color.g, _color.b, uint16_t(_color.a) * uint16_t(dustmote.get_alpha()) / 255 };
		_renderer->render(&_texture, { 0,0 }, _texture.get_psize(), dustmote.position, { dustmote.size,dustmote.size * _renderer->get_aspect_ratio() }, dustmote.position, { 0,0 }, dustmote.angular_direction * -57.295779487L);
	}
}