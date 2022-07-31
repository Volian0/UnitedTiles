#include "SongInfo.cpp"
#include "File.cpp"

#include <midifile/impl.hpp>

#include <stdexcept>
#include <iostream>
#include <string_view>

using namespace smf;

int main(int args_size, char** args)
try
{
	constexpr std::string_view filename = "/home/volian/witness2.mid";
	constexpr uint32_t single_tile_length = 48;
	constexpr long double start_tps = 5.2L;
	constexpr long double multiplier = 1.18248206376L;

	MidiFile midi;
	midi.read(filename.data());
	if (!midi.status() || midi.getTrackCount() == 0)
		throw std::runtime_error("MIDI file is invalid");
	//midi.deltaTicks();
	uint32_t tpqn = midi.getTicksPerQuarterNote();

	//uint32_t lupst = 48;
	uint32_t track_tiles = 2;
	uint32_t track_other = 3;

	const uint32_t double_tile_length = single_tile_length / 2;

	SongInfo usong;
	usong.version = 1;
	usong.length_units_per_single_tile = single_tile_length;
	usong.note_ticks_per_single_tile = single_tile_length;
	usong.starting_tempo = start_tps;
	usong.acceleration_method = SongInfo::AccelerationMethod::LINEAR;
	usong.acceleration_info.parameter = multiplier;

	uint32_t track_count = midi.getTrackCount();

	if (track_count <= track_tiles || track_count <= track_other)
		throw std::runtime_error("Invalid tracks specified");

	uint32_t tiles_count = midi[track_tiles].getEventCount();
	uint32_t other_count = midi[track_other].getEventCount();

	uint32_t other_i = 0;

	bool note_pressed = false;
	bool expect_double_tile = false;
	uint32_t last_on_tick;

	uint8_t max_velocity = 0;

	for (uint32_t tiles_i = 0; tiles_i < tiles_count; ++tiles_i)
	{
		auto& event = midi[track_tiles].getEvent(tiles_i);
		//std::cout << event.tick << " | " << event.getTempoBPM() << std::endl;
		if (event.isNoteOn())
		{
			if (note_pressed)
				throw std::runtime_error("Chords are not allowed in tiles track.");
			note_pressed = true;
			last_on_tick = event.tick;
		}
		else if (event.isNoteOff())
		{
			if (!note_pressed)
				abort(); //this shouldn't happen
			note_pressed = false;
			uint32_t length = event.tick - last_on_tick;
			auto& tile = usong.tiles.emplace_back();
			// for the first tile let's put All Notes Off event
			static bool only_one_off = false;
			if (!only_one_off)
			{
				tile.note_events.emplace(0, NoteEvent(NoteEvent::Type::ALL_OFF));
				only_one_off = true;
			}
			tile.length = length;
			for (;other_i < other_count;++other_i)
			{
				auto& other_event = midi[track_other].getEvent(other_i);
				if (other_event.tick >= event.tick)
				{
					//--other_i;
					break;
				}
				if (!other_event.isNote())
					continue;
				uint32_t relative_tick = other_event.tick - last_on_tick;
				NoteEvent note_event;
				note_event.type = other_event.isNoteOn() ? NoteEvent::Type::ON : NoteEvent::Type::OFF;
				note_event.key = other_event.getKeyNumber();
				note_event.velocity = other_event.getVelocity();
				//update max velocity
				max_velocity = std::max(note_event.velocity, max_velocity);

				tile.note_events.emplace(relative_tick, note_event);
				//std::cout << relative_tick << std::endl;
			}
			//single tile
			if (length == single_tile_length)
			{
				if (expect_double_tile)
					throw std::runtime_error("Expected 2nd double tile, got single tile");
				tile.type = TileInfo::Type::SINGLE;
			}
			//double tile
			else if (length == double_tile_length)
			{
				if (!expect_double_tile)
				{
					expect_double_tile = true;
					tile.type = TileInfo::Type::DOUBLE;
					tile.length *= 2;
				}
				else
				{
					expect_double_tile = false;
					auto note_events = std::move(tile.note_events);
					usong.tiles.pop_back();
					usong.tiles.back().note_events_2nd_tile = std::move(note_events);
				}
			}
			//long tile
			else if (length > single_tile_length)
			{
				std::cout << length << ' ' << std::flush;
				if (expect_double_tile)
					throw std::runtime_error("Expected 2nd double tile, got long tile");
				tile.type = TileInfo::Type::LONG;
			}
			else
			{
				throw std::runtime_error("Invalid length");
			}
		}
	}
	if (expect_double_tile)
		throw std::runtime_error("Expected 2nd double tile, got nothing");

	// normalize volume of tiles (max = 127)
	for (auto& tile : usong.tiles)
	{
		for (auto& [delta, event] : tile.note_events)
		{
			if (event.type == NoteEvent::Type::ON)
			{
				event.velocity = std::uint16_t(event.velocity) * 127 / max_velocity;
			}
		}
		if (tile.type == TileInfo::Type::DOUBLE)
		{
			for (auto& [delta, event] : tile.note_events_2nd_tile)
			{
				if (event.type == NoteEvent::Type::ON)
				{
					event.velocity = std::uint16_t(event.velocity) * 127 / max_velocity;
				}
			}
		}
	}
	auto output_file = open_ofile("output.usong").value();
	usong.to_file(output_file);
}
catch (const std::exception& e)
{
	std::cout << "Exception:\n" << e.what() << std::endl;
	return -1;
}