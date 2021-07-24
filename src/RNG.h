#pragma once

#include "Types.h"

namespace RNG
{
	[[nodiscard]] extern Number number(Number min, Number max);
	[[nodiscard]] extern uint32_t integer(uint32_t min, uint32_t max);
	[[nodiscard]] extern bool boolean();
}