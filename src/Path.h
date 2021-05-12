#pragma once

#include <string>

#include "Unique.h"

namespace Path
{
	std::string user(const std::string& filename, const std::string& folder = {});
	std::string res(const std::string& filename, const std::string& folder = {});
};

class ExtractedRes : Unique
{
public:
	ExtractedRes(const std::string& filename, const std::string& folder = {});
	~ExtractedRes();

	const std::string& get_path() const;

private:
	std::string _path;
	static uint32_t _count;
};