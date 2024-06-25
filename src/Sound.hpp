#pragma once

#include <SDL_mixer.h>

#include <cstdint>
#include <string_view>

class Sound final
{
public:
    Sound(std::string_view t_file);
    ~Sound() noexcept;

    void play(std::uint_fast8_t t_channel);

    Sound(const Sound&) = delete;
    Sound(Sound&&) = delete;
    Sound& operator=(const Sound&) = delete;
    Sound& operator=(Sound&&) = delete;

private:
    Mix_Chunk* m_chunk;
};