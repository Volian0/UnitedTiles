#pragma once

#include <SDL_mixer.h>

#include <string_view>

class Music final
{
public:
    static constexpr auto MILLISECONDS_IN_SECOND{1000.0F};

    Music(std::string_view t_file);
    ~Music() noexcept;

    void play();
    void fade_in(float t_seconds);
    static void fade_out(float t_seconds);

    Music(const Music&) = delete;
    Music(Music&&) = delete;
    Music& operator=(const Music&) = delete;
    Music& operator=(Music&&) = delete;

private:
    Mix_Music* m_music;
};