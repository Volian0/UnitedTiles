#include "StateSongSelection.h"

#include "Game.h"
#include "StateDevMenu.h"
#include "File.h"
#include "Path.h"
#include "StateLevel.h"
#include "ui/Label.h"
#include "RNG.h"

#include <algorithm>

uint32_t StateSongSelection::song_index = 0;

StateSongSelection::StateSongSelection(Game* game_)
	:GameState(game_)
{
	_font = std::make_unique<Font>(game->renderer.get(), "roboto.ttf", 8.0L);
	_font2 = std::make_unique<Font>(game->renderer.get(), "roboto.ttf", 4.0L);
	_bg = std::make_unique<Texture>(game->renderer.get(), "dev_background.png");
	white_texture = std::make_unique<Texture>(game->renderer.get(), "white.png");
	_dev_button_texture = std::make_unique<Texture>(game->renderer.get(), "dev_button.png");
	_dev_button_texture->blend_mode = 1;

	ExtractedRes song_list_res("songs.db", "database");
	auto song_list_file = open_ifile(song_list_res.get_path()).value();
	song_database = song_list_file;
	song_user_database.load_from_file();
	left_button    = std::make_unique<DevButton>(Vec2{-0.8, 0.85}, 0.125, 0, "<<"  , _dev_button_texture.get(), _font.get(), this);
	right_button   = std::make_unique<DevButton>(Vec2{0.8, 0.85}, 0.125, 0, ">>"  , _dev_button_texture.get(), _font.get(), this);
	confirm_button = std::make_unique<DevButton>(Vec2{0, 0.85}, 0.5, 0, "Play", _dev_button_texture.get(), _font.get(), this);
	return_button = std::make_unique<DevButton>(Vec2{0, -0.85}, 0.5, 0, "Return", _dev_button_texture.get(), _font.get(), this);

	text_input = std::make_unique<TextInput>(std::string{},0.9,Vec2{0.0L, -0.55}, _font.get(), this, white_texture.get());
}

void StateSongSelection::update()
{
	if (text_input->update())
	{
		found_songs.clear();
		std::map<uint16_t, std::string> names_and_composers;
		for (const auto& [s_id, s_info] : song_database.songs_infos)
		{
			names_and_composers[s_id] = s_info.name + " ";
			for (const auto& [c_id, c_info] : song_database.composers_infos)
			{
				for (const auto& sc_id : s_info.composer_ids)
				{
					if (c_id == sc_id)
					{
						names_and_composers[s_id] += c_info.name;
					}
				}
			}
			std::transform(names_and_composers[s_id].begin(), names_and_composers[s_id].end(), names_and_composers[s_id].begin(),
    		[](unsigned char c){ return std::tolower(c); });
		}
		std::string search_result = text_input->get_text();
		std::transform(search_result.begin(), search_result.end(), search_result.begin(),
    	[](unsigned char c){ return std::tolower(c); });
		for (const auto& [id, str] : names_and_composers)
		{
			if (str.find(search_result) != std::string::npos)
			{
				found_songs.emplace(id);
			}
		}
	}
	if (left_button->update())
	{
		if (song_index == 0)
		{
			song_index = song_database.songs_infos.size();
		}
		song_index--;
		if (!found_songs.empty())
		{
			while (!found_songs.count(song_database.songs_infos.at(song_index).first))
			{
				if (song_index == 0)
				{
					song_index = song_database.songs_infos.size();
				}
				song_index--;
			}
		}
	}
	if (right_button->update())
	{
		song_index++;
		if (song_index == song_database.songs_infos.size())
		{
			song_index = 0;
		}
	}
	if (confirm_button->update())
	{
		return game->change_state<StateLevel>(uint16_t(song_database.songs_infos.at(song_index).first));
	}
	if (return_button->update())
	{
		return game->change_state<StateDevMenu>();
	}
	if (!found_songs.empty())
	{
		while (!found_songs.count(song_database.songs_infos.at(song_index).first))
		{
			song_index++;
			if (song_index == song_database.songs_infos.size())
			{
				song_index = 0;
			}
		}
	}
}

void StateSongSelection::render() const
{
	game->renderer->render(_bg.get(), { 0,0 }, _bg->get_psize(), { 0,0 }, { 1,1 }, { 0,0 });
	text_input->render();
	left_button->render();
	right_button->render();
	confirm_button->render();
	return_button->render();

	const auto& song_info = song_database.songs_infos.at(song_index).second;
	std::string song_name = song_info.name;
	std::string composers;
	std::vector<std::string_view> composer_list;
	for (auto composer_id : song_info.composer_ids)
	{
		for (const auto& composer : song_database.composers_infos)
		{
			if (composer.first == composer_id)
			{
				composer_list.emplace_back(composer.second.name);
			}
		}
	}

	for (uint8_t i = 0; i < composer_list.size(); ++i)
	{
		composers += composer_list[i];
		if (i != composer_list.size() - 1)
		{
			composers += ", ";
		}
	}

	Label lsong_name(song_name, 0.9L, {0.0L, -0.1L - 0.25L}, {}, _font.get(), game->renderer.get());
	Label lcomposers(composers, 0.9L, {0.0L, 0.05L - 0.25L}, {}, _font2.get(), game->renderer.get());
	lcomposers.label_text_texture->tint = Colors::GRAY;
	lsong_name.render(game->renderer.get());
	lcomposers.render(game->renderer.get());
	if (song_user_database.scores.count(song_database.songs_infos.at(song_index).first))
	{
		const auto& score = song_user_database.scores.at(song_database.songs_infos.at(song_index).first);
		Label llap(std::to_string(score.reached_lap), 0.45L, {-0.5, 0.25L+0.1L}, {}, _font.get(), game->renderer.get());
		Label lscore(std::to_string(score.reached_score), 0.45L, {0.5, 0.25L+0.1L}, {}, _font.get(), game->renderer.get());
		Label llap2("Lap reached", 0.45L, {-0.5, 0.25L-0.05L}, {}, _font2.get(), game->renderer.get());
		Label lscore2("High Score", 0.45L, {0.5, 0.25L-0.05L}, {}, _font2.get(), game->renderer.get());
		llap2.label_text_texture->tint = Colors::GRAY;
		lscore2.label_text_texture->tint = Colors::GRAY;
		lscore.label_text_texture->tint = {255, 64, 64, 255};
		llap.render(game->renderer.get());
		lscore.render(game->renderer.get());
		llap2.render(game->renderer.get());
		lscore2.render(game->renderer.get());
	}



	//Texture song_name(game->renderer.get(), _font.get(), song_database.songs_infos.at(song_index).second.name, { 255, 255, 255, 255 });
	//Texture composer_name(game->renderer.get(), _font.get(), "by " + song_list.songs.at(song_index).composer, { 255, 255, 255, 255 });
	//Texture file_name(game->renderer.get(), _font.get(), "(filename: "+song_list.songs.at(song_index).filename+")", { 127, 127, 127, 200 });
	//game->renderer->render(&song_name, { 0,0 }, song_name.get_psize(), { 0,0 }, song_name.get_rsize(), { 0,0 });
	//game->renderer->render(&composer_name, { 0,0 }, composer_name.get_psize(), { 0,song_name.get_rsize().y }, composer_name.get_rsize() * 0.5L, { 0,0 }, { 0,-1 });
	//game->renderer->render(&file_name, { 0,0 }, file_name.get_psize(), { 0,-song_name.get_rsize().y }, file_name.get_rsize() * 0.5L, { 0,0 }, {0,1});
}
