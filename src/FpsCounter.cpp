#include "FpsCounter.h"

uint32_t FpsCounter::update(const Timepoint& timepoint)
{
	timepoints.emplace(timepoint);
	timepoints.erase(timepoints.begin(), timepoints.lower_bound(timepoint - std::chrono::seconds(1)));
	return timepoints.size();
}