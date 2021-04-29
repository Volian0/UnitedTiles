#include "StateTest.h"

#include "Game.h"
#include "Renderer.h"

#include <SDL/SDL.h>

StateTest::StateTest(Game* game_) : GameState(game_)
{
	_font = std::make_unique<Font>(game->renderer.get(), "roboto.ttf", 20.0L);
	_bg = std::make_unique<Texture>(game->renderer.get(), "bg.png");
}

void StateTest::update()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
		{
			return game->stop();
		}
		else if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
		{
			game->renderer->reload();
		}
	}
	_text = std::make_unique<Texture>(game->renderer.get(), _font.get(), std::to_string(_fps_counter.update()), glm::u8vec4{ 63,255,63,255 });
}

void StateTest::render() const
{
	game->renderer->render(_bg.get(), { 0,0 }, _bg->get_psize(), { 0,0 }, { 1,1 }, { 0,0 });
	game->renderer->render(_text.get(), { 0,0 }, _text->get_psize(), { 0,0 }, _text->get_rsize(), { 0,0 });
}
