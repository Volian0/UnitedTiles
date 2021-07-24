#include "AudioDevice.h"

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio/miniaudio.h>

#include <atomic>

std::atomic_bool active_audio = true;

void AudioDevice::data_callback(ma_device* device, void* output, const void* input, uint32_t frames)
{
	if (!active_audio)
		return;

	AudioDevice& audio_device = *reinterpret_cast<AudioDevice*>(device->pUserData);
	std::scoped_lock lock(audio_device._soundfont_mutex);
	if (audio_device._soundfont)
	{
		audio_device._soundfont->render(frames, output);
	}
}

AudioDevice::AudioDevice(uint16_t sample_rate_, bool stereo_)
: sample_rate{sample_rate_}, stereo{stereo_}
{
	_ptr = new ma_device;
	
	ma_device_config device_cfg = ma_device_config_init(ma_device_type_playback);
	device_cfg.playback.format = ma_format_s16;
	device_cfg.playback.channels = stereo ? 2 : 1;
	device_cfg.sampleRate = sample_rate;
	device_cfg.dataCallback = data_callback;
	device_cfg.pUserData = this;

	ma_device_init(NULL, &device_cfg, reinterpret_cast<ma_device*>(_ptr));
	ma_device_start(reinterpret_cast<ma_device*>(_ptr));
}

AudioDevice::~AudioDevice()
{
	ma_device_stop(reinterpret_cast<ma_device*>(_ptr));
	ma_device_uninit(reinterpret_cast<ma_device*>(_ptr));

	delete reinterpret_cast<ma_device*>(_ptr);
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
