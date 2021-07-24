#include "RNG.h"

#include <random>

namespace RNG
{
	static std::mt19937 generator{ std::random_device()() };

	Number number(Number min, Number max)
	{
		return std::uniform_real_distribution<Number>(min, max)(generator);
	}

	uint32_t integer(uint32_t min, uint32_t max)
	{
		return std::uniform_int_distribution<uint32_t>(min, max)(generator);
	}

	bool boolean()
	{
		static std::uniform_int_distribution<uint16_t> distribution(0, 1);
		return distribution(generator);
	}
}
