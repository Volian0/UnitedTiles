#pragma once

#include "GameState.h"
#include "SongInfo.h"
#include "DevButton.h"
#include "ui/TextInput.h"
#include "ui/ScrollablePanel.h"
#include "ui/Label.h"

#include <memory>
#include <array>
#include <string_view>
#include <deque>

struct SongPanel
{
    //std::string_view song_name;
    //std::string_view composer_name;
    //Texture txt_song_name;
    Label song_name;
    Label composer_name;
    //std::optional<Label> year;
    std::optional<Label> score;
    std::optional<Label> lap;
    uint8_t medal_level; //0=black, 1=bronze, 2=silver, 3=gold, 4=diamond
    uint16_t song_id;
    DevButton play_button;
    //DevButton details_button;

    void change_position(Number y_pos);
    void render(class StateSongMenu& state_song_menu) const;
    bool search(std::string_view search_string) const;
};

class StateSongMenu : public GameState
{
    friend SongPanel;

public:
	StateSongMenu(Game* game_);

private:
	void update() override;
	void render() const override;

	SongDatabase song_database;
	SongUserDatabase song_user_database;

    Font font32;
    Font font24;
    std::array<std::optional<Label>, 4> medals_texts;
    mutable Texture txt_white;
    mutable std::array<Texture, 4> medal_textures;
    mutable Texture glass_icon;
    mutable Texture settings_gear;
    std::deque<uint16_t> allowed_song_ids;
    std::vector<SongPanel> song_panels;
    Texture _dev_button_texture;
    TextInput input_song;
    ScrollablePanel spanel;
    DevButton settings_button;

    [[nodiscard]] constexpr auto get_x_pos(Number t_pos) const noexcept -> Number
    {
        return t_pos / 256.0L - 1.0L;
    }

    [[nodiscard]] constexpr auto get_x_size(Number t_size) const noexcept -> Number
    {
        return t_size / 512.0L;
    }

    [[nodiscard]] constexpr auto get_y_size(Number t_size, Number t_aspect_ratio) const noexcept -> Number
    {
        return t_size * t_aspect_ratio / 512.0L;
    }

    [[nodiscard]] constexpr auto get_y_pos(Number t_pos, Number t_aspect_ratio, Number t_offset = 0.0L) const noexcept -> Number
    {
        return t_pos * t_aspect_ratio / 256.0L - 1.0L + t_offset;
    }

    //std::vector<Texture> song_names;
    //std::vector<Texture> composer_names;
};