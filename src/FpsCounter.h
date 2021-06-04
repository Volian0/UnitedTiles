#pragma once

#include "Timepoint.h"

#include <set>

class FpsCounter
{
	std::multiset<Timepoint> timepoints;
public:
	uint32_t update(const Timepoint& timepoint = {});
};