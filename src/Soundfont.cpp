#include "Soundfont.h"

#include "Path.h"

#include <fluidsynth/fluidsynth.h>

Soundfont::Soundfont(const std::string& filename)
{
	_ptr_settings = new_fluid_settings();
	fluid_settings_setnum(reinterpret_cast<fluid_settings_t*>(_ptr_settings), "synth.gain", 1.0L);
	_ptr = new_fluid_synth(reinterpret_cast<fluid_settings_t*>(_ptr_settings));
	ExtractedRes soundfont_file(filename, "soundfonts");
	fluid_synth_sfload(reinterpret_cast<fluid_synth_t*>(_ptr), soundfont_file.get_path().c_str(), true);
}

Soundfont::~Soundfont()
{
	delete_fluid_synth(reinterpret_cast<fluid_synth_t*>(_ptr));
	delete_fluid_settings(reinterpret_cast<fluid_settings_t*>(_ptr_settings));
}

void Soundfont::cc(uint8_t control, uint8_t value)
{
	fluid_synth_cc(reinterpret_cast<fluid_synth_t*>(_ptr), 0, control, value);
}

void Soundfont::note_on(uint8_t key, uint8_t velocity)
{
	fluid_synth_noteon(reinterpret_cast<fluid_synth_t*>(_ptr), 0, key, velocity);
}

void Soundfont::note_off(uint8_t key)
{
	fluid_synth_noteoff(reinterpret_cast<fluid_synth_t*>(_ptr), 0, key);
}

void Soundfont::reset()
{
	fluid_synth_program_reset(reinterpret_cast<fluid_synth_t*>(_ptr));
}

void Soundfont::render(uint32_t frames, void* buffer)
{
	fluid_synth_write_s16(reinterpret_cast<fluid_synth_t*>(_ptr), frames, buffer, 0, 2, buffer, 1, 2);
}

