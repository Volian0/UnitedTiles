#include "Soundfont.h"

#include "Path.h"
#include "Types.h"

#define TSF_IMPLEMENTATION
#include <TinySoundFont/tsf.h>

#include <iostream>

Soundfont::Soundfont(const std::string& filename, uint16_t sample_rate, bool stereo)
{
	ExtractedRes soundfont_file(filename, "soundfonts");
	std::scoped_lock lock(_mutex); //unsafe?
	_ptr = tsf_load_filename(soundfont_file.get_path().c_str());
	tsf_set_output(reinterpret_cast<tsf*>(_ptr), stereo ? TSF_STEREO_INTERLEAVED : TSF_MONO, sample_rate); //sample rate
	tsf_set_volume(reinterpret_cast<tsf*>(_ptr), 1.0L);
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
	tsf_note_on(reinterpret_cast<tsf*>(_ptr), 0, key, Number(velocity) / 127.0L);
	if (_notes_on.count(key))
	{
		std::cout << "Duplicated note on: " << unsigned(key) << std::endl;
	}
	_notes_on.emplace(key);
}

void Soundfont::note_off(uint8_t key)
{
	std::scoped_lock lock(_mutex);
	tsf_note_off(reinterpret_cast<tsf*>(_ptr), 0, key);
	_notes_on.erase(key);
}

void Soundfont::all_notes_off()
{
	std::scoped_lock lock(_mutex);
	tsf_note_off_all(reinterpret_cast<tsf*>(_ptr));
	_notes_on.clear();
}

void Soundfont::reset()
{
	std::scoped_lock lock(_mutex);
	tsf_reset(reinterpret_cast<tsf*>(_ptr));
	_notes_on.clear();
}

void Soundfont::render(uint32_t frames, void* buffer)
{
	play_events();
	std::scoped_lock lock(_mutex);
	tsf_render_short(reinterpret_cast<tsf*>(_ptr), reinterpret_cast<short*>(buffer), frames, 0);
}

void Soundfont::add_event(const Timepoint& timepoint, const NoteEvent& event)
{
	std::scoped_lock lock(_mutex_events);
	if (!_events.empty() && timepoint < _events.rbegin()->first)
	{
		std::cout << "WARNING: Fixing soundfont event position!" << std::endl;
		_events.emplace(_events.rbegin()->first, event);
	}
	else
	{
		_events.emplace(timepoint, event);
	}
}

void Soundfont::play_event(const NoteEvent& event)
{
	if (event.type == NoteEvent::Type::ALL_OFF)
	{
		all_notes_off();
	}
	else if (event.type == NoteEvent::Type::ON)
	{
		note_on(event.key, event.velocity);
	}
	else if (event.type == NoteEvent::Type::OFF)
	{
		note_off(event.key);
	}
}

void Soundfont::play_all_events()
{
	std::scoped_lock lock(_mutex_events);
	for (const auto& [timepoint, event] : _events)
	{
		play_event(event);
	}
	_events.clear();
}

void Soundfont::play_events(const Timepoint& bound)
{
	std::scoped_lock lock(_mutex_events);
	auto lower_bound = _events.lower_bound(bound);
	for (auto it = _events.begin(); it != lower_bound; ++it)
	{
		play_event(it->second);
	}
	_events.erase(_events.begin(), lower_bound);
}

uint8_t Soundfont::get_notes_on_size()
{
	std::scoped_lock lock(_mutex_events);
	return _notes_on.size();
}