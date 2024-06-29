#pragma once

#include "../NonCopyable.h"
#include "../Types.h"
#include "../GameState.h"
#include "../Texture.h"
#include "../Font.h"
#include "Label.h"
#include "ScrollablePanel.h"

#include <iostream>

class TextInput
{
    public:
    TextInput(std::string starting_text_, Number max_width_, Vec2 position_, Font* font_, GameState* state_, Texture* white_, uint16_t max_size_ = 64);
    ~TextInput() noexcept;

    bool update();
    void render() const;

    bool append_text();

    const std::string& get_text() const noexcept;

    [[nodiscard]] inline bool in_range(Vec2 position) const noexcept
    {
        const auto& width = m_max_width;
        const auto& height = get_height();
        const auto& origin = position;
        if (-m_position.x + origin.x > -width && -m_position.x + origin.x < width)
        if (-m_position.y + origin.y > -height && -m_position.y + origin.y < height)
        {
            return true;
        }
        return false;
    }

    [[nodiscard]] inline bool has_focus() const noexcept
    {
        return this == m_current_focused_text_input; 
    }

    Number get_height() const noexcept;

    Vec2 m_position;
    std::string m_text;

private:
    Texture* m_white_txt;
    Number m_max_width;
    
    Font* m_font;
    GameState* m_state;
    std::unique_ptr<Label> m_label;
    std::vector<uint8_t> m_sizes;
    uint16_t m_max_size;
    static TextInput* m_current_focused_text_input;
};