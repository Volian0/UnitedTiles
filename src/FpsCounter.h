#pragma once

#include "Timepoint.h"

#include <set>
#include <map>

class FpsCounter
{
	std::multimap<Timepoint, Number> timepoints;
	//            ^tp        ^delta
public:
	[[nodiscard]] uint32_t update(const Timepoint& timepoint = {});
	[[nodiscard]] uint32_t get_low_fps() const;
};