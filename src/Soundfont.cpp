#include "Soundfont.h"

#include "Path.h"
#include "Types.h"

#define TSF_IMPLEMENTATION
#include <TinySoundFont/tsf.h>

#include <iostream>

Soundfont::Soundfont(const std::string& filename, uint16_t sample_rate, bool stereo)
:m_stereo{stereo}
{
	std::scoped_lock lock(_mutex); //unsafe?
	if (filename.empty())
	{
		ExtractedRes soundfont_file("test.sf2", "soundfonts");
		_ptr = tsf_load_filename(soundfont_file.get_path().c_str());
	}
	else
	{
		_ptr = tsf_load_filename(filename.c_str());
	}
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
	if (_notes_on.count(key))
	{
		if (always_sustain)
		{
			tsf_note_off(reinterpret_cast<tsf*>(_ptr), 0, key);
		}
		else
		{
			std::cout << "Duplicated note on: " << unsigned(key) << std::endl;
		}
	}
	tsf_note_on(reinterpret_cast<tsf*>(_ptr), 0, key, Number(velocity) / 127.0L);
	_notes_on.emplace(key);
}

void Soundfont::note_off(uint8_t key)
{
	if (always_sustain)
		return;
	std::scoped_lock lock(_mutex);
	tsf_note_off(reinterpret_cast<tsf*>(_ptr), 0, key);
	_notes_on.erase(key);
}

void Soundfont::all_notes_off()
{
	if (always_sustain)
		return;
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
	Timepoint tp;
	Timepoint tp_end{tp + static_cast<long double>(frames)/44100.0L};
	std::scoped_lock lock(_mutex_events);
	
	auto lower_bound = _events.lower_bound(tp_end);
	int frames_rendered{0};
	auto update_delta = [&](int delta_frames){
		if (delta_frames > frames_rendered)
		{
			std::scoped_lock lock(_mutex);
			const auto frames_to_render = delta_frames - frames_rendered;
			//std::cout << "rendering frames: " << frames_to_render << std::endl;
			tsf_render_short(reinterpret_cast<tsf*>(_ptr), reinterpret_cast<short*>(buffer)+frames_rendered*(m_stereo ? 2 : 1), frames_to_render, 0);
			frames_rendered = delta_frames;
		}
	};
	{ 
		for (auto it = _events.begin(); it != lower_bound; ++it)
		{
			auto delta = it->first - tp;
			//std::cout << "rendering event with delta frames " << delta << std::endl;
			//std::cout << "done playing event" << std::endl;
			int delta_frames = delta * 44100.0L;
			//std::cout << "rendering event with delta frames " << delta_frames << std::endl;
			update_delta(delta_frames);
			play_event(it->second);
		}
		update_delta(frames);
	}
	_events.erase(_events.begin(), lower_bound);
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

void Soundfont::set_volume(Number volume)
{
	std::scoped_lock lock(_mutex);
	tsf_set_volume(reinterpret_cast<tsf*>(_ptr), volume);
}