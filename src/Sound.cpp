#include "Sound.hpp"

#include <stdexcept>

Sound::Sound(std::string_view t_file) : m_chunk{Mix_LoadWAV(t_file.data())}
{
}

Sound::~Sound() noexcept
{
    Mix_FreeChunk(m_chunk);
}

void Sound::play(std::uint_fast8_t t_channel)
{
    Mix_PlayChannel(t_channel, m_chunk, 0);
}