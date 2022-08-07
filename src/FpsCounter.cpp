#include "FpsCounter.h"

#include <limits>

uint32_t FpsCounter::update(const Timepoint& timepoint)
{
	Number delta = 0.0L;
	//calculate delta here
	if (!timepoints.empty())
	{
		delta = timepoint - timepoints.rbegin()->first;
	}
	timepoints.emplace(timepoint, delta);
	timepoints.erase(timepoints.begin(), timepoints.lower_bound(timepoint - 1.0L));
	return timepoints.size();
}

uint32_t FpsCounter::get_low_fps() const
{
	Number longest_delta = 0.0L;
	for (const auto& [tp, delta] : timepoints)
	{
		if (delta > longest_delta)
		{
			longest_delta = delta;
		}
	}
	Number min_fps = 1.0L / longest_delta;
	if (min_fps >= Number(std::numeric_limits<uint32_t>::max()))
	{
		return std::numeric_limits<uint32_t>::max();
	}
	return min_fps;
}