#include "Game.h"

#include <SDL.h> //This is required here, so don't remove it

int main(int argc, char* argv[])
{
	std::string_view song_filename;
	if (argc > 1)
	{
		song_filename = argv[1];
	}
	Game game{song_filename};
	game.run();
	return 0; //required
}