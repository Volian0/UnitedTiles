#pragma once

#include "Timepoint.h"

#include <set>

class FpsCounter
{
	std::multiset<Timepoint> timepoints;
public:
	[[nodiscard]] uint32_t update(const Timepoint& timepoint = {});
};