#include "Soundfont.h"

#include "Path.h"
#include "Number.h"

#define TSF_IMPLEMENTATION
#include <TinySoundFont/tsf.h>

Soundfont::Soundfont(const std::string& filename)
{
	std::scoped_lock lock(_mutex);
	ExtractedRes soundfont_file(filename, "soundfonts");
	//reinterpret_cast<tsf*>(_ptr)
	_ptr = tsf_load_filename(soundfont_file.get_path().c_str());
	tsf_set_output(reinterpret_cast<tsf*>(_ptr), TSF_STEREO_INTERLEAVED, 44100); //sample rate
	tsf_set_volume(reinterpret_cast<tsf*>(_ptr), 2.0L);
}

Soundfont::~Soundfont()
{
	std::scoped_lock lock(_mutex);
	tsf_close(reinterpret_cast<tsf*>(_ptr));
}

void Soundfont::cc(uint8_t control, uint8_t value)
{
	std::scoped_lock lock(_mutex);
	tsf_channel_midi_control(reinterpret_cast<tsf*>(_ptr), 0, control, value);
}

void Soundfont::note_on(uint8_t key, uint8_t velocity)
{
	std::scoped_lock lock(_mutex);
	tsf_note_on(reinterpret_cast<tsf*>(_ptr), 0, key, Number(velocity) / 255.0L);
}

void Soundfont::note_off(uint8_t key)
{
	std::scoped_lock lock(_mutex);
	tsf_note_off(reinterpret_cast<tsf*>(_ptr), 0, key);
}

void Soundfont::reset()
{
	std::scoped_lock lock(_mutex);
	tsf_reset(reinterpret_cast<tsf*>(_ptr));
}

void Soundfont::render(uint32_t frames, void* buffer)
{
	std::scoped_lock lock(_mutex);
	tsf_render_short(reinterpret_cast<tsf*>(_ptr), reinterpret_cast<short*>(buffer), frames, 0);
}

