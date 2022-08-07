#include "Configuration.h"

#include "Path.h"
#include "File.h"

#include <algorithm>
#include <filesystem>
#include <iostream>

void Configuration::load_from_file()
{
	auto file = open_ifile(Path::user("config.cfg"));
	if (!file)
	{
		return;
	}
	audio_sample_rate = std::clamp<uint16_t>(read_u16(*file).value_or(44100), 10000, 50000);
	audio_stereo = read_u8(*file).value_or(1);
	enable_particles_burst = read_u8(*file).value_or(1);
	enable_particles_dustmotes = read_u8(*file).value_or(1);
	enable_hit_markers = read_u8(*file).value_or(0);
	show_fps = read_u8(*file).value_or(0);
	show_gameplay_debug_info = read_u8(*file).value_or(0);
	enable_vsync = read_u8(*file).value_or(1);
	god_mode = read_u8(*file).value_or(0);
	fake_lag = read_u8(*file).value_or(0);
}

void Configuration::save_to_file() const
{
	{
		auto file = open_ofile(Path::user("temp_config.cfg"));
		write_u16(*file, audio_sample_rate);
		write_u8(*file, audio_stereo);
		write_u8(*file, enable_particles_burst);
		write_u8(*file, enable_particles_dustmotes);
		write_u8(*file, enable_hit_markers);
		write_u8(*file, show_fps);
		write_u8(*file, show_gameplay_debug_info);
		write_u8(*file, enable_vsync);
		write_u8(*file, god_mode);
		write_u8(*file, fake_lag);
		file->close();
		if (!file.value())
		{
			std::cout << "error with config!" << std::endl;
			std::abort();
		}
	}
	std::filesystem::rename(Path::user("temp_config.cfg"), Path::user("config.cfg"));
}

void Configuration::reset_to_default()
{
	Configuration default_config;
	*this = default_config;
}
