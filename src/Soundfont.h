#pragma once

#include "Unique.h"

#include <string>
#include <mutex>

class Soundfont : Unique
{
public:
	Soundfont(const std::string& filename);
	~Soundfont();

	void cc(uint8_t control, uint8_t value);
	void note_on(uint8_t key, uint8_t velocity);
	void note_off(uint8_t key);
	void reset();

	void render(uint32_t frames, void* buffer);

private:
	std::mutex _mutex;
	void* _ptr;
};