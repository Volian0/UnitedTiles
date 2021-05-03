#include "Path.h"

#include <SDL/SDL.h>

std::string Path::res(const std::string& filename, const std::string& folder)
{
	std::string path = folder + "/" + filename;
#ifndef __ANDROID__
	path = "res/" + path;
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
	static std::string user_path = get_pref_path();
	return user_path + folder + "/" + filename;
}