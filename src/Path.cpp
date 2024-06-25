#include "Path.h"

#include <SDL.h>

#include <filesystem>

std::string get_base_path()
{
	char* base_path = SDL_GetBasePath();
	std::string path = base_path;
	SDL_free(base_path);
	return path;
}

std::string Path::res(const std::string& filename, const std::string& folder)
{
	std::string path = folder + "/" + filename;
#ifndef __ANDROID__
	static const std::string base_path = get_base_path();
	path = base_path + "res/" + path;
#endif
	return path;
}

std::string get_pref_path()
{
	char* user_path = SDL_GetPrefPath("Volian", "United Tiles");
	std::string pref_path(user_path);
	SDL_free(user_path);
	return pref_path;
}

std::string Path::user(const std::string& filename, const std::string& folder)
{
	static const std::string user_path = get_pref_path();
	return user_path + folder + "/" + filename;
}

ExtractedRes::ExtractedRes(const std::string& filename, const std::string& folder)
{
	std::filesystem::create_directories(Path::user({}, "extracted/" + folder));
	++_count;
	_path = Path::user(filename, "extracted/" + folder);
	auto ifile = SDL_RWFromFile(Path::res(filename, folder).c_str(), "rb");
	auto ofile = SDL_RWFromFile(_path.c_str(), "wb");
	auto size = SDL_RWsize(ifile);
	while (true)
	{
		char buffer[512];
		auto read_n = SDL_RWread(ifile, buffer, 1, sizeof(buffer));
		if (read_n == 0)
		{
			break;
		}
		if (SDL_RWwrite(ofile, buffer, 1, read_n) != read_n)
		{
			abort();
		}
	}
	SDL_RWclose(ifile);
	SDL_RWclose(ofile);
}

ExtractedRes::~ExtractedRes()
{
	std::filesystem::remove(_path);
	--_count;
	if (_count == 0)
	{
		std::filesystem::remove_all(Path::user({}, "extracted"));
	}
}

uint32_t ExtractedRes::_count = 0;

const std::string& ExtractedRes::get_path() const
{
	return _path;
}
