#include "StateSoundfonts.h"

#include "Game.h"
#include "File.h"
#include "Path.h"
#include "StateSettings.h"

#include <algorithm>
#include <map>
#include <string_view>
#include <string>

SoundfontPanel::SoundfontPanel(StateSoundfonts* state_, uint16_t id_)
:id{id_ + 1},
name  {Label{extra_soundfonts.at(id_).name.data(),   state_->get_x_size(380), Vec2{state_->get_x_pos(106), 0}, Vec2{-1,0}, &state_->font32, state_->game->renderer.get()}},
author{Label{extra_soundfonts.at(id_).author.data(), state_->get_x_size(220), Vec2{state_->get_x_pos(106), 0}, Vec2{-1,0}, &state_->font24, state_->game->renderer.get()}},
mb_size{Label{std::to_string(extra_soundfonts.at(id_).mb_size) + " MB", state_->get_x_size(220), Vec2{state_->get_x_pos(326), 0}, Vec2{1, 0}, &state_->font24, state_->game->renderer.get()}},
info{},
b_download{DevButton{{state_->get_x_pos(454), 0}, state_->get_x_size(64), 0, " ", &state_->_dev_button_texture, &state_->font24, state_, 0.0625L * 0.75L}},
b_cancel{DevButton{{state_->get_x_pos(454), 0}, state_->get_x_size(64), 0, " ", &state_->_dev_button_texture, &state_->font24, state_, 0.0625L * 0.75L}},
b_delete{DevButton{{state_->get_x_pos(374), 0}, state_->get_x_size(64), 0, " ", &state_->_dev_button_texture, &state_->font24, state_, 0.0625L * 0.75L}},
b_use{DevButton{{state_->get_x_pos(454), 0}, state_->get_x_size(64), 0, " ", &state_->_dev_button_texture, &state_->font24, state_, 0.0625L * 0.75L}},
b_unuse{DevButton{{state_->get_x_pos(454), 0}, state_->get_x_size(64), 0, " ", &state_->_dev_button_texture, &state_->font24, state_, 0.0625L * 0.75L}},
network_file{NetworkFileWrapper{extra_soundfonts.at(id_).url.data(), std::to_string(id) + ".sf2", extra_soundfonts.at(id_).hash.data()}}
{
    author.label_text_texture->tint  = {128, 128, 128, 255};
    mb_size.label_text_texture->tint = {96, 96, 96, 255};
}

template<typename T, std::size_t N, std::size_t... I>
constexpr auto create_array_impl(StateSoundfonts* s, std::index_sequence<I...>) {
    return std::array<T, N>{ T{s, I}... };
}

template<typename T, std::size_t N>
constexpr auto create_array(StateSoundfonts* s) {
    return create_array_impl<T, N>(s, std::make_index_sequence<N>{});
}

StateSoundfonts::StateSoundfonts(Game* game_)
:GameState(game_),
    _dev_button_texture{game->renderer.get(), "dev_button.png"},
    txt_white{game->renderer.get(), "white.png"},
    font32{game->renderer.get(), "roboto.ttf", 6.25L * 0.75L},
    font24{game->renderer.get(), "roboto.ttf", 4.6875L * 0.75L},
    settings_gear{game->renderer.get(), "gear.png"},
    settings_button{Vec2{get_x_pos(482.0L), 0.0L}, get_x_size(40.0L), 0, " ", &_dev_button_texture, &font24, this, 0.0625L * 0.625L},
    txt_icons{game->renderer.get(), "misc_icons.png"},
    m_certs{"cacert.pem","misc"},
    panels{
        create_array<SoundfontPanel, extra_soundfonts.size()>(this)
    }
{
    settings_gear.blend_mode = 1;
    txt_icons.blend_mode = 1;
    _dev_button_texture.blend_mode = 1;
    spanel.size.x = 1.0L;
    spanel.origin.y = -1.0L;
    spanel.max_offset = 0.0L;
    spanel._state = this;
    spanel.position.x = 0.0L;
    txt_icons.tint = {16, 16, 16, 255};
}

void StateSoundfonts::update()
{
    const auto aspect_ratio = game->renderer->get_aspect_ratio();

    settings_button.position.y = get_y_pos(30, aspect_ratio);
    if (settings_button.update())
    {
        return game->change_state<StateSettings>();
    }

    const uint16_t song_n = panels.size();
    const Number all_size_y = get_y_size(130.0L * song_n/* - 10.0L*/, aspect_ratio);
    const Number topbar_size = get_y_size(118.0L, aspect_ratio);
    //const Number footbar_size = get_y_size(64.0L + 4.0L, aspect_ratio);
    const Number footbar_size = 0.0L;
    const Number visible_size_y = 1.0L - (topbar_size + footbar_size);
    spanel.size.y = visible_size_y;
    spanel.position.y = topbar_size * 2.0L - 1.0L;

    spanel.min_offset = std::min((visible_size_y - all_size_y) * 2.0L, 0.0L);

    const auto scrolled = spanel.update(aspect_ratio);
    uint32_t current_y_pixel_position = 118;
    const Number scroll_offset = spanel.get_offset();
    for (auto& panel : panels)
    {
        constexpr uint32_t composer_y_pos = 52;
        constexpr uint32_t songname_y_pos = 24;
        constexpr uint32_t score_y_pos = 88;
        constexpr uint32_t play_y_pos = 88;

        panel.name.position.y = get_y_pos(songname_y_pos + current_y_pixel_position, aspect_ratio, scroll_offset);
        panel.author.position.y = get_y_pos(composer_y_pos + current_y_pixel_position, aspect_ratio, scroll_offset);
        panel.mb_size.position.y = get_y_pos(score_y_pos + current_y_pixel_position, aspect_ratio, scroll_offset);

        const std::array<DevButton*, 5> buttons
        {&panel.b_download,&panel.b_cancel,&panel.b_delete,&panel.b_use,&panel.b_unuse};

        for (auto button : buttons)
        {
            button->position.y = get_y_pos(play_y_pos + current_y_pixel_position, aspect_ratio, scroll_offset);
            if (spanel.is_scrolled() || scrolled)
                button->clear_held();
        }

        const auto file_info = panel.network_file.get_info();
        bool clear_buttons = false;

        if (file_info.can_download())
        {
            if (clear_buttons = panel.b_download.update())
            {
                panel.network_file.download();
            }
        }

        else if (file_info.can_cancel())
        {
            if (clear_buttons = panel.b_cancel.update())
            {
                panel.network_file.cancel();
            }
        }

        else
        {
            if (clear_buttons = panel.b_delete.update())
            {
                panel.network_file.remove();
                if (game->cfg->preferred_soundfont == panel.id)
                {
                    game->cfg->preferred_soundfont = 0;
                }
            }
            else
            {
                if (game->cfg->preferred_soundfont == panel.id)
                {
                    if (clear_buttons = panel.b_unuse.update())
                    {
                        game->cfg->preferred_soundfont = 0;
                    }
                }
                else
                {
                    if (clear_buttons = panel.b_use.update())
                    {
                        game->cfg->preferred_soundfont = panel.id;
                    }
                }
            }
        }

        if (clear_buttons)
        {
            for (auto button : buttons)
                button->clear_held();
        }

        current_y_pixel_position += 130;
    }
}

void StateSoundfonts::render() const
{
    const auto aspect_ratio = game->renderer->get_aspect_ratio();
    txt_white.tint = {16, 16, 16, 255};
    game->renderer->render(&txt_white, {}, txt_white.get_psize(),
    {}, {1.0L, 1.0L}, {});

    const Vec2 top_medal_size{get_x_size(40),get_y_size(40,aspect_ratio)};
    const Vec2 icon_size{get_x_size(32),get_y_size(32,aspect_ratio)};

    game->renderer->render(&settings_gear, {}, settings_gear.get_psize(),
        settings_button.position, top_medal_size, {});

    spanel.start_rendering();
    const Vec2 dark_square_size{get_x_size(80), get_y_size(120,aspect_ratio)};
    const Vec2 light_square_size{get_x_size(412), get_y_size(120,aspect_ratio)};
    for (auto& panel : panels)
    {
        const Vec2 zero_point{-1.0L, panel.name.position.y - get_y_pos(24, aspect_ratio, 1.0L)};

        if (zero_point.y >= 1.0L)
        {
            continue;
        }

        const auto file_info = panel.network_file.get_info();

        const Vec2 square_position{zero_point.x + get_x_pos(90) + 1.0L,zero_point.y + get_y_pos(60,aspect_ratio, 1.0L)};

        /*txt_white.tint = file_info.exists() ? (game->cfg->preferred_soundfont == panel.id ? Color{64, 128, 64, 255} : Color{32, 32, 32, 255})
        : (file_info.failed() ? Color{64, 32, 32, 255} : Color{32, 32, 32, 255});
        game->renderer->render(&txt_white, {}, txt_white.get_psize(),
        square_position, dark_square_size, {}, {1.0L, 0.0L});*/

        Color tint_square = {0x20, 0x20, 0x20, 0xFF};
        Color tint_progress = Colors::BLACK;

        if (file_info.state == NetworkFileWrapper::State::DOWNLOADING)
        {
            tint_square = {0x14, 0x28, 0x28, 0xFF};
        }
        else if (file_info.state == NetworkFileWrapper::State::FAILED)
        {
            tint_square = {0x30, 0x18, 0x18, 0xFF};
        }
        else if (file_info.exists() && game->cfg->preferred_soundfont == panel.id)
        {
            tint_square = {0x14, 0x28, 0x14, 0xFF};
            //tint_square += tint_square;
        }
        tint_progress = tint_square + tint_square;
        tint_progress.a = 0xFF;

        txt_white.tint = tint_square;
        game->renderer->render(&txt_white, {}, txt_white.get_psize(),
        square_position, dark_square_size, {}, {1.0L, 0.0L});
        //if (file_info.state == NetworkFileWrapper::State::DOWNLOADING || file_info.state == NetworkFileWrapper::State::FAILED)
        {
            txt_white.tint = tint_progress;
            game->renderer->render(&txt_white, {}, txt_white.get_psize(),
            {square_position.x, square_position.y + get_y_size(120.0L, aspect_ratio)}, {dark_square_size.x, dark_square_size.y * file_info.progress}, {}, {1.0L, 1.0L});
        }
        //if (file_info.state == NetworkFileWrapper::State::DOWNLOADING || file_info.state == NetworkFileWrapper::State::FAILED)
        //{

        //}

        txt_white.tint = {48, 48, 48, 255};
        game->renderer->render(&txt_white, {}, txt_white.get_psize(),
        square_position, light_square_size, {}, {-1.0L, 0.0L});

        panel.name.render(game->renderer.get());
        panel.author.render(game->renderer.get());
        panel.mb_size.render(game->renderer.get());

        std::string label_text;
        //Label{std::to_string(extra_soundfonts.at(id_).mb_size) + " MB", state_->get_x_size(220), Vec2{state_->get_x_pos(326 - 220), 0}, Vec2{-1, 0}, &state_->font24, state_->game->renderer.get()}}
        switch(file_info.state)
        {
            case NetworkFileWrapper::State::DOWNLOADING:
            {
                label_text = "Downloading... " + std::to_string(std::clamp<uint8_t>(file_info.progress * 100.0L, 0, 99)) + "%";
            }
            break;
            case NetworkFileWrapper::State::FAILED:
            {
                label_text = "Download failed!";
            }
            break;
        }
        if (!label_text.empty())
        {
            const auto label = Label{label_text, get_x_size(220), Vec2{get_x_pos(326 - 220), panel.mb_size.position.y}, Vec2{-1, 0}, &font24, game->renderer.get()};
            label.render(game->renderer.get());
        }

        if (file_info.can_download())
        {
            //render download button
            panel.b_download.render();
            game->renderer->render(&txt_icons, {0, 0}, {64, 64},
            panel.b_download.position, icon_size, {});

        }

        else if (file_info.can_cancel())
        {
            //render cancel button
            panel.b_cancel.render();
            game->renderer->render(&txt_icons, {64, 0}, {64, 64},
            panel.b_cancel.position, icon_size, {});
        }

        else
        {
            //render delete button
            panel.b_delete.render();
            game->renderer->render(&txt_icons, {0, 64}, {64, 64},
            panel.b_delete.position, icon_size, {});

            if (game->cfg->preferred_soundfont == panel.id)
            {
                //render unuse button
                panel.b_unuse.render();
                game->renderer->render(&txt_icons, {128, 64}, {64, 64},
                panel.b_download.position, icon_size, {});
            }
            else
            {
                //render use button
                panel.b_use.render();
                game->renderer->render(&txt_icons, {64, 64}, {64, 64},
                panel.b_download.position, icon_size, {});
            }
        }
    }
    spanel.stop_rendering();
}