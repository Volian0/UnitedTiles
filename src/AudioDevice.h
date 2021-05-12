#pragma once

#include "Unique.h"
#include "Soundfont.h"

#include <memory>
#include <mutex>

class AudioDevice : Unique
{
public:
	AudioDevice();
	~AudioDevice();

	void load_soundfont(const std::shared_ptr<Soundfont>& soundfont);
	std::shared_ptr<Soundfont> load_soundfont(const std::string& filename);
	void unload_soundfont();

	static void data_callback(struct ma_device* device, void* output, const void* input, uint32_t frames);

private:
	void* _ptr;
	std::shared_ptr<Soundfont> _soundfont;
	std::mutex _soundfont_mutex;
};