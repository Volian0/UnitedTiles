#include "Timepoint.h"

Timepoint::Timepoint()
	:_timepoint{std::chrono::steady_clock::now()}
{
}

Timepoint& Timepoint::operator+=(Number duration)
{
	_timepoint = std::chrono::time_point_cast<std::chrono::steady_clock::duration>(_timepoint + std::chrono::duration<Number>(duration));
	return *this;
}

Timepoint Timepoint::operator+(Number duration) const
{
	return Timepoint(*this) += duration;
}

Timepoint& Timepoint::operator-=(Number duration)
{
	return *this += -duration;
}

Timepoint Timepoint::operator-(Number duration) const
{
	return Timepoint(*this) -= duration;
}

Number Timepoint::operator-(const Timepoint& previous_timepoint) const
{
	return std::chrono::duration<Number>(_timepoint - previous_timepoint._timepoint).count();
}

Number Timepoint::operator%(Number duration) const
{
	return std::chrono::duration<Number>(_timepoint.time_since_epoch() % std::chrono::duration_cast<std::chrono::steady_clock::duration>(std::chrono::duration<Number>(duration))).count();
}