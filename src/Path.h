#pragma once

#include <string>

#include "Unique.h"

namespace Path
{
	std::string user(const std::string& filename, const std::string& folder = {});
	std::string res(const std::string& filename, const std::string& folder = {});
};