#include "GameState.h"
#include "SongInfo.h"
#include "DevButton.h"
#include "NetworkFile.h"
#include "Configuration.h"
#include "ExtraSoundfonts.h"
#include "ui/TextInput.h"
#include "Path.h"
#include "ui/ScrollablePanel.h"
#include "ui/Label.h"

#include <memory>
#include <array>
#include <string_view>
#include <deque>

struct SoundfontPanel
{
    SoundfontPanel(class StateSoundfonts* state_, uint16_t id_);
    uint16_t id;
    Label name;
    Label author;
    Label mb_size;
    mutable std::optional<Label> info; //expensive to update
    DevButton b_download;
    DevButton b_cancel;
    DevButton b_delete;
    DevButton b_use;
    DevButton b_unuse;
    NetworkFileWrapper network_file;
    //first, we update the positions
    //then, we check the state and update the buttons based on state
    //when we render, we also check the state and update the info label
};

class StateSoundfonts : public GameState
{
    friend SoundfontPanel;
public:
	StateSoundfonts(Game* game_);

    void update() override;
    void render() const override;

    Font font32;
    Font font24;

    mutable Texture txt_white;
    mutable Texture settings_gear;
    mutable Texture txt_icons;

    Texture _dev_button_texture;
    ScrollablePanel spanel;
    DevButton settings_button;

    DevButton volume_minus;
    DevButton volume_plus;

    DevButton volume_minus_small;
    DevButton volume_plus_small;

    ExtractedRes m_certs;

    std::array<SoundfontPanel, extra_soundfonts.size()> panels;

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
};