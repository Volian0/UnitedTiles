#pragma once

#include "Unique.h"

#include <string>

class Soundfont : Unique
{
public:
	Soundfont(const std::string& filename);
	~Soundfont();

	void cc(uint8_t control, uint8_t value);
	void sustain(uint8_t value);
	void note_on(uint8_t key, uint8_t velocity);
	void note_off(uint8_t key);
	void all_notes_off();
	void reset();

private:
	void* _ptr;
};