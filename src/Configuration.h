#pragma once

#include <cstdint>

class Configuration
{
public:
	void load_from_file();
	void save_to_file() const;
	void reset_to_default();

	constexpr static uint16_t audio_sample_rate = 44100;
	bool audio_stereo = true; //false = mono
	constexpr static bool enable_particles_burst = false;
	constexpr static bool enable_particles_dustmotes = true;
	constexpr static bool enable_hit_markers = false;
	constexpr static bool show_fps = false;
	constexpr static bool show_gameplay_debug_info = false;
	bool enable_vsync = true;
	bool god_mode = false;
	constexpr static bool fake_lag = false;
	uint16_t preferred_soundfont = 0; // 0 = default soundfont
	constexpr static bool three_section_mode = true;
	constexpr static bool show_tps_instead_of_score = false;
	constexpr static bool sustain_mode = false;
	constexpr static bool show_sdl_errors = false;
	uint8_t soundfont_volume = 128; // x^3 / 2097152
	constexpr static bool limit_note_velocity = false; 
};
