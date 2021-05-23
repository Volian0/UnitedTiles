#include "RNG.h"

#include <random>

namespace RNG
{
	static std::mt19937 generator{ std::random_device()() };

	Number number(Number min, Number max)
	{
		std::uniform_real_distribution<Number> distribution(min, max);
		return distribution(generator);
	}

	uint32_t integer(uint32_t min, uint32_t max)
	{
		std::uniform_int_distribution<uint32_t> distribution(min, max);
		return distribution(generator);
	}
}
