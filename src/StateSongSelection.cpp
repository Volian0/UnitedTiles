#include "StateSongSelection.h"

#include "Game.h"
#include "StateDevMenu.h"
#include "File.h"
#include "Path.h"

uint32_t StateSongSelection::song_index = 0;

StateSongSelection::StateSongSelection(Game* game_)
	:GameState(game_)
{
	_font = std::make_unique<Font>(game->renderer.get(), "roboto.ttf", 8.0L);
	_bg = std::make_unique<Texture>(game->renderer.get(), "dev_background.png");
	_dev_button_texture = std::make_unique<Texture>(game->renderer.get(), "dev_button.png");
	_dev_button_texture->blend_mode = 1;

	ExtractedRes song_list_res("songs.txt", "database");
	auto song_list_file = open_ifile(song_list_res.get_path()).value();
	song_list = song_list_file;
	left_button    = std::make_unique<DevButton>(Vec2{-0.8, 0.85}, 0.125, 0, "<<"  , _dev_button_texture.get(), _font.get(), this);
	right_button   = std::make_unique<DevButton>(Vec2{0.8, 0.85}, 0.125, 0, ">>"  , _dev_button_texture.get(), _font.get(), this);
	confirm_button = std::make_unique<DevButton>(Vec2{0, 0.85}, 0.5, 0, "Play", _dev_button_texture.get(), _font.get(), this);
	return_button = std::make_unique<DevButton>(Vec2{0, -0.85}, 0.5, 0, "Return", _dev_button_texture.get(), _font.get(), this);
}

void StateSongSelection::update()
{
	if (left_button->update())
	{
		if (song_index == 0)
		{
			song_index = song_list.songs.size();
		}
		song_index--;
	}
	if (right_button->update())
	{
		song_index++;
		if (song_index == song_list.songs.size())
		{
			song_index = 0;
		}
	}
	if (confirm_button->update())
	{

	}
	if (return_button->update())
	{
		return game->change_state<StateDevMenu>();
	}
}

void StateSongSelection::render() const
{
	game->renderer->render(_bg.get(), { 0,0 }, _bg->get_psize(), { 0,0 }, { 1,1 }, { 0,0 });
	left_button->render();
	right_button->render();
	confirm_button->render();
	return_button->render();
	Texture song_name(game->renderer.get(), _font.get(), song_list.songs.at(song_index).name, { 255, 255, 255, 255 });
	Texture composer_name(game->renderer.get(), _font.get(), "by " + song_list.songs.at(song_index).composer, { 255, 255, 255, 255 });
	Texture file_name(game->renderer.get(), _font.get(), "(filename: "+song_list.songs.at(song_index).filename+")", { 127, 127, 127, 200 });
	game->renderer->render(&song_name, { 0,0 }, song_name.get_psize(), { 0,0 }, song_name.get_rsize(), { 0,0 });
	game->renderer->render(&composer_name, { 0,0 }, composer_name.get_psize(), { 0,song_name.get_rsize().y }, composer_name.get_rsize() * 0.5L, { 0,0 }, { 0,-1 });
	game->renderer->render(&file_name, { 0,0 }, file_name.get_psize(), { 0,-song_name.get_rsize().y }, file_name.get_rsize() * 0.5L, { 0,0 }, {0,1});
}
