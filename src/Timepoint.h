#pragma once

#include "Types.h"

#include <chrono>

class Timepoint
{
public:
	//create a timepoint that represents the current point in time
	Timepoint();

	//add duration to the timepoint
	Timepoint& operator+=(Number duration);
	Timepoint operator+(Number duration) const;

	//subtract duration from the timepoint
	Timepoint& operator-=(Number duration);
	Timepoint operator-(Number duration) const;

	//create a duration by subtracting the previous timepoint
	Number operator-(const Timepoint& previous_timepoint) const;

	//compare with the other timepoint
	bool operator<(const Timepoint& timepoint) const;

	Number operator%(Number duration) const;

private:
	std::chrono::time_point<std::chrono::steady_clock> _timepoint;
};