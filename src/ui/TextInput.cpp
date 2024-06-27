#include "TextInput.h"
#include "../Game.h"

#include <SDL.h>

#include <iostream>

TextInput::TextInput(std::string starting_text_, Number max_width_, Vec2 position_, Font* font_, GameState* state_, Texture* white_, uint16_t max_size_)
:m_text{std::move(starting_text_)}, m_max_width{max_width_}, m_position{position_}, m_font{font_}, m_state{state_}, m_max_size{max_size_}, m_white_txt{white_}
{
    if (!m_text.empty())
    {
        m_label = std::make_unique<Label>(m_text, m_max_width, m_position, Vec2{-1.0L, 0.0L}, m_font, m_state->game->renderer.get());
        m_sizes.push_back(m_text.size());
    }
}

struct TextInputFocus
{

};

TextInput::~TextInput()
{
    if (has_focus())
    {
        m_current_focused_text_input = nullptr;
        SDL_StopTextInput();
    }
}

bool TextInput::update()
{
    bool updated = false;
    //std::cout << "update" << std::endl;
    if (has_focus())
    {
        updated = append_text();
        if (m_state->pressed_backspace)
        {
            if (!m_text.empty())
            {
                for (uint8_t i = 0; i < m_sizes.back(); ++i)
                    m_text.pop_back();
                m_sizes.pop_back();
                if (m_text.empty())
                {
                    m_label.reset();
                }
                else
                    m_label = std::make_unique<Label>(m_text, m_max_width, m_position, Vec2{-1.0L, 0.0L}, m_font, m_state->game->renderer.get());
                updated = true;
            }
        }
        else if (m_state->pressed_enter)
        {
            m_current_focused_text_input = nullptr;
            SDL_StopTextInput();
            //updated = true;
        }
    }
    for (const auto& touch_event : m_state->touch_events)
    {
        if (touch_event.type == TouchEvent::Type::DOWN)
        {
            if (has_focus() && !in_range(touch_event.position))
            {
                //disable input
                m_current_focused_text_input = nullptr;
                SDL_StopTextInput();
            }
            if (!has_focus() && in_range(touch_event.position))
            {
                m_state->game->sfx_tick->play(0);
                if (m_current_focused_text_input == nullptr)
                {
                    SDL_StartTextInput();
                }
                m_current_focused_text_input = this;
            }
        }
    }
    return updated;
}

void TextInput::render() const
{
    //std::cout << "render" << std::endl;
    auto renderer = m_state->game->renderer.get();
    //std::cout << "zzz" << std::endl;
        m_white_txt->tint = {64, 64, 64, 255};
    //std::cout << "zzz2" << std::endl;
    if (has_focus())
        m_white_txt->tint = {128, 64, 64, 255};
    //std::cout << "ok" << std::endl;
    //renderer->render(m_white_txt, {}, m_white_txt->get_psize(), m_position, {m_max_width,get_height()}, {});
    //std::cout << "ok2" << std::endl;
    if (m_label)
    {
        //std::cout << "ok3" << std::endl;
        m_label->position = m_position;
        m_label->position.x -= m_max_width;
        m_label->render(renderer);
    }
}

Number TextInput::get_height() const noexcept
{
    return m_state->game->renderer->get_aspect_ratio() * 24.0L / 512.0L;
 }

bool TextInput::append_text()
{
    if (!m_state->key_events.empty() && m_state->key_events.size() + m_text.size() <= m_max_size)
    {
        m_sizes.emplace_back(m_state->key_events.size());
        m_text += m_state->key_events;
        m_label = std::make_unique<Label>(m_text, m_max_width, m_position, Vec2{-1.0L, 0.0L}, m_font, m_state->game->renderer.get());
        return true;
    }
    return false;
}

const std::string& TextInput::get_text() const noexcept
{
    return m_text;
}

TextInput* TextInput::m_current_focused_text_input = nullptr;