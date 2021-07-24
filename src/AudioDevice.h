#pragma once

#include "NonCopyable.h"
#include "Soundfont.h"

#include <memory>
#include <mutex>

class AudioDevice : NonCopyable
{
public:
	AudioDevice(uint16_t sample_rate_, bool stereo_);
	~AudioDevice();

	[[deprecated]] void load_soundfont(const std::shared_ptr<Soundfont>& soundfont);
	[[nodiscard]] std::shared_ptr<Soundfont> load_soundfont(const std::string& filename);
	void unload_soundfont();

	static void data_callback(struct ma_device* device, void* output, const void* input, uint32_t frames);

	const uint16_t sample_rate;
	const bool stereo;

private:
	void* _ptr;
	std::shared_ptr<Soundfont> _soundfont;
	mutable std::mutex _soundfont_mutex;
};