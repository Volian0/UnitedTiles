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
	bool enable_particles_burst = false;
	bool enable_particles_dustmotes = true;
	bool enable_hit_markers = false;
	bool show_fps = false;
	bool show_gameplay_debug_info = false;
	bool enable_vsync = true;
	bool god_mode = false;
	bool fake_lag = false;
	uint16_t preferred_soundfont = 0; // 0 = default soundfont
	bool three_section_mode = false;
	bool show_tps_instead_of_score = false;
	bool sustain_mode = false;
	bool show_sdl_errors = true;
	uint8_t soundfont_volume = 128; // x^3 / 2097152
	bool limit_note_velocity = false;
};
