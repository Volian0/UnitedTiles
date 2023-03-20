#include "StateSongMenu.h"

#include "Game.h"
#include "File.h"
#include "Path.h"
#include "StateLevel.h"
#include "ui/Label.h"
#include "RNG.h"
#include "StateSettings.h"

#include <algorithm>
#include <map>
#include <string_view>

std::string StateSongMenu::last_search{}; 
Number StateSongMenu::last_position{0.0L};
std::optional<std::uint16_t> StateSongMenu::last_song{};

StateSongMenu::StateSongMenu(Game* game_)
	:GameState(game_),
    _dev_button_texture{game->renderer.get(), "dev_button.png"},
    txt_white{game->renderer.get(), "white.png"},
    font32{game->renderer.get(), "roboto.ttf", 6.25L * 0.75L},
    font24{game->renderer.get(), "roboto.ttf", 4.6875L * 0.75L},
    //font16{game->renderer.get(), "roboto.ttf", 3.125L * 0.75L},
    medal_textures{Texture{game->renderer.get(), "medal_bronze.png"}, Texture{game->renderer.get(), "medal_silver.png"},Texture{game->renderer.get(), "medal_gold.png"},Texture{game->renderer.get(), "cup.png"}},
    input_song{last_search, get_x_size(432), {get_x_pos(274), 0.0L}, &font24, this, &txt_white},
    settings_gear{game->renderer.get(), "gear.png"},
    glass_icon{game->renderer.get(), "glass.png"},
    settings_button{Vec2{get_x_pos(482.0L), 0.0L}, get_x_size(40.0L), 0, " ", &_dev_button_texture, &font24, this, 0.0625L * 0.625L}
{
    settings_gear.blend_mode = 1;
    glass_icon.blend_mode = 1;
    glass_icon.tint = {96, 96, 96, 255};
	_dev_button_texture.blend_mode = 1;
    for (auto& txt : medal_textures)
    {
        txt.blend_mode = 1;
    }
    medal_textures[3].tint.r = 192;
	ExtractedRes song_list_res("songs.db", "database");
	auto song_list_file = open_ifile(song_list_res.get_path()).value();
	song_database = song_list_file;
	song_user_database.load_from_files();

    std::map<uint16_t, std::string_view> composer_names;
    const auto& song_scores = song_user_database.scores;
    for (const auto& [composer_id, composer_info] : song_database.composers_infos)
    {
        composer_names.emplace(composer_id, composer_info.name);
    }
    std::array<uint16_t, 4> medal_n{0,0,0,0};
    for (const auto& [song_id, song_info] : song_database.songs_infos)
    {
        auto& song_panel = song_panels.emplace_back(SongPanel{
            Label{song_info.name, get_x_size(380), Vec2{get_x_pos(106), 0}, Vec2{-1,0}, &font32, game->renderer.get()},
            Label{composer_names.at(song_info.composer_ids.at(0)).data(), get_x_size(220), Vec2{get_x_pos(106), 0}, Vec2{-1,0}, &font24, game->renderer.get()},
            {},
            {},
            0,
            song_id,
            {{get_x_pos(414), 0}, get_x_size(144), 0, (last_song && *last_song == song_id) ? "Replay" : "Play", &_dev_button_texture, &font32, this, 0.0625L * 0.75L},
            song_info.unlockable_type,
            song_info.unlockable_amount 
        });
        song_panel.play_button.spanel = &spanel;
        if (song_scores.count(song_id))
        {
            const auto& score = song_scores.at(song_id);
            //song_panel.medal_level = 4;
            const uint32_t completed_laps = std::max<int64_t>(int64_t(score.reached_lap) - 1, 0);
            song_panel.medal_level = std::min<int64_t>(completed_laps, 3);
            if (score.got_perfect_score)
            {
                song_panel.medal_level = 4;
            }
            song_panel.score.emplace(std::to_string(score.reached_score), get_x_size(60), Vec2{get_x_pos(266), 0}, Vec2{-1,0}, &font24, game->renderer.get());
            song_panel.score->label_text_texture->tint = {96, 96, 96, 255};
            for (uint8_t i = 0; i < 4; ++i)
            {
                if (song_panel.medal_level > i)
                    ++medal_n[i];
            }
            //if (song_panel.medal_level)
            //    ++medal_n[song_panel.medal_level - 1];
            if (completed_laps > 3)
            {
                song_panel.lap.emplace(std::to_string(completed_laps), get_x_size(24), Vec2{get_x_pos(78), 0}, Vec2{0,0}, &font32, game->renderer.get());
                song_panel.lap->label_text_texture->tint = {0xB0, 0x96, 0x01, 0xFF};
            }
        }
        /*if (song_info.create_year)
        {
            song_panel.year.emplace(std::to_string(song_info.create_year), get_x_size(224), Vec2{get_x_pos(90), 0}, Vec2{-1,-1}, &font16, game->renderer.get());
            song_panel.year->label_text_texture->tint = {96, 96, 96, 255};
        }*/
        song_panel.composer_name.label_text_texture->tint = {128, 128, 128, 255};
    }
    for (auto& panel : song_panels)
    {
        if (panel.req_type)
        {
            if (medal_n.at(panel.req_type - 1) < panel.req_amount)
            {
                //panel.locked = true;
                panel.req_amount_label.emplace(std::to_string(panel.req_amount), get_x_size(48), Vec2{get_x_pos(418), 0}, Vec2{-1,0}, &font32, game->renderer.get());
            }
        }
    }
    for (uint8_t i = 0; i < 4; ++i)
    {
        medals_texts[i].emplace(std::to_string(medal_n[i]), get_x_size(55), Vec2{get_x_pos(81.5L + Number(i) * 113.0L), 0.0L}, Vec2{0,0}, &font24, game->renderer.get());
    }
    spanel.size.x = 1.0L;
    spanel.origin.y = -1.0L;
    spanel.max_offset = 0.0L;
    spanel._state = this;
    spanel.position.x = 0.0L;

    need_to_restore = true;
    tp_state_start = {};
}

void StateSongMenu::update()
{

    const auto aspect_ratio = game->renderer->get_aspect_ratio();

    settings_button.position.y = get_y_pos(30, aspect_ratio);
    if (settings_button.update())
    {
        return game->change_state<StateSettings>();
    }

    const std::array<Vec2, 4> top_label_positions{
        Vec2{get_x_pos(0) + 1.0L,get_y_pos(30,aspect_ratio)},
        Vec2{get_x_pos(0) + 1.0L,get_y_pos(30,aspect_ratio)},
        Vec2{get_x_pos(0) + 1.0L,get_y_pos(30,aspect_ratio)},
        Vec2{get_x_pos(0) + 1.0L,get_y_pos(30,aspect_ratio)}
    };

    for (uint8_t i = 0; i < 4; ++i)
    {
        medals_texts[i]->position.y = get_y_pos(30, aspect_ratio);
    }

    input_song.m_position.y = get_y_pos(84, aspect_ratio);

    //first, let's update the searchbox
    if (input_song.update() || need_to_restore)
    {
        /*bool last_bit = false;
        static const auto remove_utf8{[](const std::string& t_string){
            std::string string;
            for (unsigned char c : t_string)
            {
                if (c < 128)
                {
                    std::cout << int(c) << " ";
                    string += c;
                }
                else
                {
                    c -= 128;
                    if (c < 0b01000000)
                    {
                        std::cout << int(c) << " ";
                        string += c;
                    }
                    last_bit = c & 0b00000001;
                }
            }
            std::cout << std::endl << string << std::endl;
            return string;
        }};*/
        last_search = (input_song.get_text());
        spanel.reset();
        //preform the search here!
        allowed_song_ids.clear();
        //
        std::vector<std::pair<uint16_t, std::string>> names_and_composers;
		for (const auto& [s_id, s_info] : song_database.songs_infos)
		{
            names_and_composers.emplace_back(s_id, (s_info.name) + " ");
			for (const auto& [c_id, c_info] : song_database.composers_infos)
			{
				for (const auto& sc_id : s_info.composer_ids)
				{
					if (c_id == sc_id)
					{
						names_and_composers.back().second += (c_info.name) + " ";
					}
				}
			}
			std::transform(names_and_composers.back().second.begin(), names_and_composers.back().second.end(), names_and_composers.back().second.begin(),
    		[](unsigned char c){ return std::tolower(c); });
		}
		std::string search_result = input_song.get_text();
		std::transform(search_result.begin(), search_result.end(), search_result.begin(),
    	[](unsigned char c){ return std::tolower(c); });
		for (const auto& [id, str] : names_and_composers)
		{
			if (str.find(search_result) != std::string::npos)
			{
				allowed_song_ids.emplace_back(id);
			}
		}
        //
        //std::cout << allowed_song_ids.size() << std::endl;
    }

    //calculate max offset for scrollable panel
    const bool all_songs = allowed_song_ids.empty();
    const uint16_t song_n = all_songs ? song_panels.size() : allowed_song_ids.size();
    const Number all_size_y = get_y_size(130.0L * song_n/* - 10.0L*/, aspect_ratio);
    const Number topbar_size = get_y_size(118.0L, aspect_ratio);
    //const Number footbar_size = get_y_size(64.0L + 4.0L, aspect_ratio);
    const Number footbar_size = 0.0L;
    const Number visible_size_y = 1.0L - (topbar_size + footbar_size);
    spanel.size.y = visible_size_y;
    spanel.position.y = topbar_size * 2.0L - 1.0L;


    //then, let's set max offset and update the scrollable panel
    spanel.min_offset = std::min((visible_size_y - all_size_y) * 2.0L, 0.0L);
    if (need_to_restore)
    {
        spanel.set_offset(last_position);
    }
    const auto scrolled = spanel.update(aspect_ratio);

    //next, let's set the perfect positions for everything
    uint32_t current_y_pixel_position = 118;
    uint16_t current_song_index = 0;
    const Number scroll_offset = spanel.get_offset();
    last_position = scroll_offset;
    for (auto& song_panel : song_panels)
    {
        if (!all_songs)
        {
            if (current_song_index == allowed_song_ids.size())
            {
                break;
            }
            if (allowed_song_ids.at(current_song_index) == song_panel.song_id)
            {
                ++current_song_index;
            }
            else
            {
                continue;
            }
        }

        //set big box
        //set smaller box
        //set medal

        //constexpr uint32_t year_y_pos = 58;
        constexpr uint32_t composer_y_pos = 52;
        constexpr uint32_t songname_y_pos = 24;
        constexpr uint32_t score_y_pos = 88;
        constexpr uint32_t play_y_pos = 88;
        constexpr uint32_t lap_y_pos = 60;

        //set lap
        if (song_panel.lap)
            song_panel.lap->position.y = get_y_pos(lap_y_pos + current_y_pixel_position, aspect_ratio, scroll_offset);

        //set composer
            song_panel.composer_name.position.y = get_y_pos(composer_y_pos + current_y_pixel_position, aspect_ratio, scroll_offset);

        //set song name
            song_panel.song_name.position.y = get_y_pos(songname_y_pos + current_y_pixel_position, aspect_ratio, scroll_offset);

        //set score
        if (song_panel.score)
            song_panel.score->position.y = get_y_pos(score_y_pos + current_y_pixel_position, aspect_ratio, scroll_offset);

        //set play button
            song_panel.play_button.position.y = get_y_pos(play_y_pos + current_y_pixel_position, aspect_ratio, scroll_offset);

        /*if (scrolled || spanel.is_scrolled())
            song_panel.play_button.clear_held();*/
        current_y_pixel_position += 130;
        if (song_panel.req_amount_label)
            continue;
        if (spanel.is_scrolled() || scrolled)
            song_panel.play_button.clear_held();
        if (song_panel.play_button.update())
	    {
            last_song = song_panel.song_id;
		    return game->change_state<StateLevel>(uint16_t(song_panel.song_id));
	    }
    }

    //finally, check button events

    need_to_restore = false;
}

void StateSongMenu::render() const
{
    const auto aspect_ratio = game->renderer->get_aspect_ratio();
    txt_white.tint = {16, 16, 16, 255};
    game->renderer->render(&txt_white, {}, txt_white.get_psize(),
    {}, {1.0L, 1.0L}, {});

    
    txt_white.tint = {32, 32, 32, 255};
    const std::array<Vec2, 4> top_medals_positions { //and rect positions too
        Vec2{get_x_pos(30),get_y_pos(30,aspect_ratio)},
        Vec2{get_x_pos(143),get_y_pos(30,aspect_ratio)},
        Vec2{get_x_pos(256),get_y_pos(30,aspect_ratio)},
        Vec2{get_x_pos(369),get_y_pos(30,aspect_ratio)}
    };

    const Vec2 top_medal_size{get_x_size(40),get_y_size(40,aspect_ratio)};
    const Vec2 locked_medal_size{get_x_size(48),get_y_size(48,aspect_ratio)};
    const Vec2 big_medal_size{get_x_size(104),get_y_size(104,aspect_ratio)};
    const Vec2 song_medal_size{get_x_size(32),get_y_size(32,aspect_ratio)};
    const Vec2 song_diamond_size{get_x_size(24),get_y_size(24,aspect_ratio)};

    //settings_button.render();
    game->renderer->render(&settings_gear, {}, settings_gear.get_psize(),
        settings_button.position, top_medal_size, {});

    for (uint8_t i = 0; i < 4; ++i)
    {
        game->renderer->render(&txt_white, {}, txt_white.get_psize(),
        top_medals_positions[i], {get_x_size(83), get_y_size(32,aspect_ratio)}, {}, {-1, 0.0L});
        
        medals_texts[i]->render(game->renderer.get());
        game->renderer->render(&medal_textures[i], {}, medal_textures[i].get_psize(),
        top_medals_positions[i], top_medal_size, {});
    }

    if (input_song.has_focus())
    {
        const Timepoint tp{};
        constexpr Number pi = 3.14159265358979323846L;
        const uint8_t pixel_color = 40.0L + 8.0L * std::sin(tp % pi * 8.0L);
        txt_white.tint = {pixel_color, pixel_color, pixel_color, 255};
    }

    game->renderer->render(&txt_white, {}, txt_white.get_psize(),
    {get_x_pos(256),get_y_pos(84,aspect_ratio)}, {get_x_size(492), get_y_size(48,aspect_ratio)}, {});

    if (spanel.is_scrolled())
    {
        txt_white.tint = {48, 48, 48, 255};
        const auto progress = spanel.get_progress();
        game->renderer->render(&txt_white, {}, txt_white.get_psize(),
        {get_x_pos(10.0L + (1.0L - progress) * 468.0L),get_y_pos(84,aspect_ratio)}, {get_x_size(24), get_y_size(48,aspect_ratio)}, {}, {-1.0L,0.0L});
    }

    game->renderer->render(&glass_icon, {}, glass_icon.get_psize(),
        {get_x_pos(34), get_y_pos(84, aspect_ratio)}, song_diamond_size, {});




    input_song.render();
    const bool all_songs = allowed_song_ids.empty();
    uint16_t current_song_index = 0;
    spanel.start_rendering();
    //only once
    //txt_white.tint = {16, 16, 64, 255};
    //game->renderer->render(&txt_white, {}, txt_white.get_psize(),
    //{}, {1.0L, 1.0L}, {});
    const Vec2 dark_square_size{get_x_size(80), get_y_size(120,aspect_ratio)};
    const Vec2 light_square_size{get_x_size(412), get_y_size(120,aspect_ratio)};
    //const Vec2 offset_dark{get_x_pos(90) + 1.0L, get_y_pos(60,aspect_ratio, 1.0L)};
    //const Vec2 offset_light{get_x_pos(90) + 1.0L, get_y_pos(2,aspect_ratio, 1.0L)};
    //const Vec2 zero_point{get_x_pos(), get_y_pos()};
    //std::cout << offset_dark.x << ", " << offset_dark.y << std::endl;
    //std::abort();
    for (auto& song_panel : song_panels)
    {
        if (!all_songs)
        {
            if (current_song_index == allowed_song_ids.size())
            {
                break;
            }
            if (allowed_song_ids.at(current_song_index) == song_panel.song_id)
            {
                ++current_song_index;
            }
            else
            {
                continue;
            }
        }
        //const Vec2 zero_point{song_panel.song_name.position.x - (get_x_pos(106) + 1.0L), song_panel.song_name.position.y - get_y_pos(24, aspect_ratio, 1.0L)};
        const Vec2 zero_point{-1.0L, song_panel.song_name.position.y - get_y_pos(24, aspect_ratio, 1.0L)};

        if (zero_point.y >= 1.0L)
        {
            continue;
        }

        const std::array<Vec2, 4> medals_positions{
            Vec2{zero_point.x + get_x_pos(122) + 1.0L,zero_point.y + get_y_pos(88,aspect_ratio, 1.0L)},
            Vec2{zero_point.x + get_x_pos(162) + 1.0L,zero_point.y + get_y_pos(88,aspect_ratio, 1.0L)},
            Vec2{zero_point.x + get_x_pos(202) + 1.0L,zero_point.y + get_y_pos(88,aspect_ratio, 1.0L)},
            Vec2{zero_point.x + get_x_pos(246) + 1.0L,zero_point.y + get_y_pos(88,aspect_ratio, 1.0L)}
        };

        const Vec2 square_position{zero_point.x + get_x_pos(90) + 1.0L,zero_point.y + get_y_pos(60,aspect_ratio, 1.0L)};

        //const Vec2 big_medal_position

        const Vec2& big_medal_position{zero_point.x + get_x_pos(78) + 1.0L,zero_point.y + get_y_pos(60,aspect_ratio, 1.0L)};

        txt_white.tint = {32, 32, 32, 255};
        game->renderer->render(&txt_white, {}, txt_white.get_psize(),
        square_position, dark_square_size, {}, {1.0L, 0.0L});

        if (song_panel.medal_level)
        {
            auto& medal_texture = medal_textures[std::min(song_panel.medal_level - 1, 2)];
            game->renderer->render(&medal_texture, {},
            medal_texture.get_psize(), big_medal_position,
            big_medal_size, {});
            if (song_panel.lap)
            {
                song_panel.lap->render(game->renderer.get());
            }
        }
        
        txt_white.tint = {48, 48, 48, 255};
        game->renderer->render(&txt_white, {}, txt_white.get_psize(),
        square_position, light_square_size, {}, {-1.0L, 0.0L});
        song_panel.composer_name.render(game->renderer.get());
        song_panel.song_name.render(game->renderer.get());
        if (song_panel.score)
            song_panel.score->render(game->renderer.get());
        if (song_panel.req_amount_label)
        {
            auto& medal_texture = medal_textures.at(song_panel.req_type - 1);
            game->renderer->render(&medal_texture, {}, medal_texture.get_psize(),
            {song_panel.play_button.position.x - get_x_size(4) * 2.0L, song_panel.play_button.position.y}, locked_medal_size, {}, {1, 0});
            song_panel.req_amount_label->position.y = song_panel.play_button.position.y;
            song_panel.req_amount_label->render(game->renderer.get());
        }
        else
            song_panel.play_button.render();
        for (uint8_t i = 0; i < 4; ++i)
        {
            const Color prev_color = medal_textures[i].tint;
            if (song_panel.medal_level < i + 1)
            {
                medal_textures[i].tint = {0, 0, 0, song_panel.req_amount_label ? 0 : 64};
            }
            game->renderer->render(&medal_textures[i], {},
            medal_textures[i].get_psize(), medals_positions[i],
            (i == 3) ? song_diamond_size : song_medal_size, {});
            medal_textures[i].tint = prev_color;
        }
    }
    spanel.stop_rendering();
    //experimental
	const Number total_time = Timepoint{} - tp_state_start;
	if (total_time < 0.25L) {
        txt_white.blend_mode = 1;
		txt_white.tint = {16, 16, 16, uint8_t(std::clamp((0.25L - total_time) * 255.0L * 4.0L, 0.0L, 255.0L))};
		game->renderer->render(&txt_white, {}, txt_white.get_psize(), {}, {1.0L, 1.0L}, {}); txt_white.blend_mode = 0;}
	//experimental
}