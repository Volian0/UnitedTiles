#pragma once

#include <chrono>

using Clock = std::chrono::steady_clock;
using Timepoint = std::chrono::time_point<Clock>;
using Duration = std::chrono::duration<std::chrono::steady_clock>;