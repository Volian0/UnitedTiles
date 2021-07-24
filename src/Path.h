#pragma once

#include <string>

#include "NonCopyable.h"

namespace Path
{
	[[nodiscard]] std::string user(const std::string& filename, const std::string& folder = {});
	[[nodiscard]] std::string res(const std::string& filename, const std::string& folder = {});
};

class ExtractedRes : NonCopyable
{
public:
	ExtractedRes(const std::string& filename, const std::string& folder = {});
	~ExtractedRes();

	[[nodiscard]] const std::string& get_path() const;

private:
	std::string _path;
	static uint32_t _count;
};