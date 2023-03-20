#pragma once

#include "NonCopyable.h"
#include "Timepoint.h"
#include "SongInfo.h"
#include "Types.h"

#include <string>
#include <mutex>
#include <map>
#include <unordered_set>
#include <array>

class Soundfont : NonCopyable
{
public:
	Soundfont(const std::string& filename, uint16_t sample_rate, bool stereo);
	~Soundfont();

	void cc(uint8_t control, uint8_t value);
	void note_on(uint8_t key, uint8_t velocity);
	void note_off(uint8_t key);
	void all_notes_off();
	void reset();

	void render(uint32_t frames, void* buffer);

	void add_event(const Timepoint& timepoint, const NoteEvent& event);
	void play_event(const NoteEvent& event);
	void play_all_events();
	void play_events(const Timepoint& bound = {});

	void set_volume(Number volume);

	uint8_t get_notes_on_size();

	bool always_sustain = false;

private:
	mutable std::mutex _mutex;
	mutable std::mutex _mutex_events;
	std::multimap<Timepoint, NoteEvent> _events;
	void* _ptr;
	std::unordered_set<uint8_t> _notes_on;
};