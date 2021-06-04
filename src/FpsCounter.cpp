#include "FpsCounter.h"

uint32_t FpsCounter::update(const Timepoint& timepoint)
{
	timepoints.emplace(timepoint);
	timepoints.erase(timepoints.begin(), timepoints.lower_bound(timepoint - 1.0L));
	return timepoints.size();
}