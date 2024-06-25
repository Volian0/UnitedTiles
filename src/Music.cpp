#include "Music.hpp"

#include <stdexcept>

Music::Music(std::string_view t_file) : m_music{Mix_LoadMUS(t_file.data())}
{
}
Music::~Music() noexcept
{
    Mix_FreeMusic(m_music);
}
void Music::play()
{
    Mix_PlayMusic(m_music, -1);
}
void Music::fade_in(float t_seconds)
{
    Mix_FadeInMusic(m_music, -1, static_cast<int>(t_seconds * MILLISECONDS_IN_SECOND));
}
void Music::fade_out(float t_seconds)
{
    Mix_FadeOutMusic(static_cast<int>(t_seconds * MILLISECONDS_IN_SECOND));
}