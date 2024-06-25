#include "AudioDevice.h"

#include <atomic>
#include <SDL.h>

#include <algorithm>

std::atomic_bool active_audio = true;

void AudioDevice::data_callback(void* t_user_ptr, std::uint8_t* t_stream, int t_length)
{
	if (!active_audio)
	{
		std::fill(t_stream, t_stream+t_length,0);
		return;
	}

	AudioDevice& audio_device = *reinterpret_cast<AudioDevice*>(t_user_ptr);
	std::scoped_lock lock(audio_device._soundfont_mutex);
	if (audio_device._soundfont)
	{
		audio_device._soundfont->render(t_length/2, t_stream);
	}
}

AudioDevice::AudioDevice(uint16_t sample_rate_, bool stereo_)
: sample_rate{sample_rate_}, stereo{stereo_}
{
	Mix_Init(MIX_INIT_OGG); 
	Mix_OpenAudio(sample_rate_, AUDIO_S16, stereo_ ? 2 : 1, 1024);
	Mix_SetPostMix(data_callback, this);
}

AudioDevice::~AudioDevice()
{
	Mix_SetPostMix(nullptr, nullptr);
	Mix_CloseAudio();
	Mix_Quit();
}

void AudioDevice::load_soundfont(const std::shared_ptr<Soundfont>& soundfont)
{
	std::scoped_lock lock(_soundfont_mutex);
	_soundfont = soundfont;
}

std::shared_ptr<Soundfont> AudioDevice::load_soundfont(const std::string& filename)
{
	std::scoped_lock lock(_soundfont_mutex);
	_soundfont.reset();
	_soundfont = std::make_shared<Soundfont>(filename, sample_rate, stereo);
	return _soundfont;
}

void AudioDevice::unload_soundfont()
{
	std::scoped_lock lock(_soundfont_mutex);
	_soundfont.reset();
}
