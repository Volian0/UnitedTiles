#pragma once

#include <cstdint>

class Configuration
{
public:
	void load_from_file();
	void save_to_file() const;
	void reset_to_default();

	uint16_t audio_sample_rate = 44100;
	bool audio_stereo = true; //false = mono
	bool enable_particles_burst = true;
	bool enable_particles_dustmotes = true;
	bool enable_hit_markers = false;
	bool show_fps = false;
	bool show_gameplay_debug_info = false;
	bool enable_vsync = true;
	bool god_mode = false;
	bool fake_lag = false;
};
