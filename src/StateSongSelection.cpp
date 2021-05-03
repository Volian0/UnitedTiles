#include "StateSongSelection.h"

#include "Game.h"
#include "StateDevMenu.h"

StateSongSelection::StateSongSelection(Game* game_)
	:GameState(game_)
{
}

void StateSongSelection::update()
{
	//return game->change_state<StateDevMenu>();
}

void StateSongSelection::render() const
{
}
