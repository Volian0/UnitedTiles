#include "StateLevel.h"

#include "File.h"
#include "Path.h"
#include "Game.h"
#include "StateSongMenu.h"
#include "ExtraSoundfonts.h"

#include "tiles/DoubleTile.h"
#include "tiles/EmptyTile.h"
#include "tiles/LongTile.h"
#include "tiles/SingleTile.h"
#include "tiles/SliderTile.h"
#include "tiles/ComboTile.hpp"

#include <picosha2/picosha2.h>

#include <algorithm>
#include <iostream>
#include <cmath>
#include <filesystem>

LevelBackground::LevelBackground(StateLevel* level)
:_level{level}, _dustmotes{
	{255, 255, 255, 127},
	level->game->renderer.get(),
	"glow.png",
	0.25, 0.5,
	3,
	8, 8,
	0, 2},
	_dustmotes_stars{
	{200, 255, 255, 127},
	_level->game->renderer.get(),
	"star.png",
	0.025, 0.05,
	8,
	2, 3,
	1, 1},
	_dustmotes_mini{
	{255, 255, 255, 63},
	level->game->renderer.get(),
	"glow.png",
	0.125, 0.25,
	2,
	2, 4,
	0, 2},
	backgrounds{Texture(level->game->renderer.get(), halloween_theme ? "bg1_a.png" : "bg1.png"), Texture(level->game->renderer.get(), halloween_theme ? "bg2_a.png" : "bg2.png"), Texture(level->game->renderer.get(), halloween_theme ? "bg3_a.png" : "bg3.png")}
{
	_dustmotes.dont_spawn = true;
	_dustmotes_stars.dont_spawn = true;
	backgrounds[1].tint.a = 0;
	backgrounds[2].tint.a = 0;
	backgrounds[1].blend_mode = 1;
	backgrounds[2].blend_mode = 1;
}

void LevelBackground::update(Number delta_time)
{
	_dustmotes.update(delta_time);
	_dustmotes_stars.update(delta_time);
	_dustmotes_mini.update(delta_time);
	if (_state >= 1 && _alpha_second < 1.0L)
	{
		_alpha_second = std::min(_alpha_second + delta_time, 1.0L);
		backgrounds[1].tint.a = _alpha_second * 255.0L;
	}
	if (_state >= 2 && _alpha_third < 1.0L)
	{
		_alpha_third = std::min(_alpha_third + delta_time, 1.0L);
		backgrounds[2].tint.a = _alpha_third * 255.0L;
	}
	_level->theme_tint.r = _alpha_second * 85.0L * 2.0L + _alpha_third * 85.0L;
	//_level->theme_tint.g = 255.0L - _alpha_second * 85.0L - _alpha_third * (42.0L);
	_level->theme_tint.g = 255.0L - _alpha_second * 85.0L + _alpha_third * (21.0L);

	if (halloween_theme)
	{
		if (_state == 0)
		{
			_level->theme_tint = {255,128,32, 255};
		}
		else if (_state == 1)
		{
			_level->theme_tint = {128,32,128, 255};
		}
		else
		{
			_level->theme_tint = {255,128,32, 255};
		}
	}

	_dustmotes.update(delta_time);
	_dustmotes_stars.update(delta_time);
}

void LevelBackground::change_state(std::uint8_t new_state)
{
	_state = new_state;
	if (_state == 1)
	{
		_dustmotes_mini.dont_spawn = true;
		_dustmotes.dont_spawn = false;
	}
	else if (_state == 2)
	{
		_dustmotes_stars.dont_spawn = false;
	}
	else
	{
		std::abort();
	}
}

void LevelBackground::render() const
{
	//render first background
	if (_alpha_second < 1.0L)
	{
		_level->game->renderer->render(&backgrounds[0], {}, backgrounds[0].get_psize(), {}, {1.0L, 1.0L}, {});
	}

	//render second background
	if (_alpha_third < 1.0L && _alpha_second > 0.0L)
	{
		_level->game->renderer->render(&backgrounds[1], {}, backgrounds[1].get_psize(), {}, {1.0L, 1.0L}, {});
	}

	//render third background
	if (_alpha_third > 0.0L)
	{
		_level->game->renderer->render(&backgrounds[2], {}, backgrounds[2].get_psize(), {}, {1.0L, 1.0L}, {});
	}
	_dustmotes_mini.render();
	_dustmotes.render();
	_dustmotes_stars.render();
}

StateLevel::StateLevel(Game* game_, uint16_t song_id_, std::string_view t_score_filename,
bool t_from_res)
	:GameState(game_),
	score_tps{this},
	score{this},
	tile_divider{game->renderer.get(), "tile_divider.png"},
	txt_single_tile{game->renderer.get(), "single_tile.png"},
	txt_game_over_tile{game->renderer.get(), "game_over_tile.png"},
	txt_single_tile_cleared{game->renderer.get(), "single_tile_cleared.png"},
	txt_long_tile{game->renderer.get(), "long_tile.png"},
	txt_long_tile_ext{game->renderer.get(), "long_tile_ext.png"},
	txt_long_tile_clearing{game->renderer.get(), "long_tile_clearing.png"},
	txt_long_tile_circle{game->renderer.get(), "long_tile_circle.png"},
	txt_long_tile_end{game->renderer.get(), "long_tile_end.png"},
	txt_arrow{std::make_optional<Texture>(game->renderer.get(), "arrow.png")},
	slider_tile{game->renderer.get(), "slider.png"},
	txt_white{game->renderer.get(), "white.png"},
	slider_tile_clearing{game->renderer.get(), "slider_clearing.png"},
	_debug_font{game->renderer.get(), "roboto.ttf", 6},
	_burst{game_->renderer.get(), "white.png"},
	//txt_trail{std::in_place, game_->renderer.get(), "trail.png"},
	lv_bg{this},
	song_id{song_id_}, 
	_dev_button_texture{ game->renderer.get(), "dev_button.png" },
	revive_yes{Vec2{}, 0.5L, 0, "Revive",  &_dev_button_texture, &_debug_font, this},
	revive_no{Vec2{}, 0.5L, 0, "Exit",  &_dev_button_texture, &_debug_font, this}
{

	if (pc_controls &&game->cfg->pc_user)
	{
		pc_keys.emplace_back(std::make_unique<Texture>(game->renderer.get(), &_debug_font, std::string{"D"}, Color{Colors::WHITE}));
		pc_keys.emplace_back(std::make_unique<Texture>(game->renderer.get(), &_debug_font, std::string{"F"}, Color{Colors::WHITE}));
		pc_keys.emplace_back(std::make_unique<Texture>(game->renderer.get(), &_debug_font, std::string{"J"}, Color{Colors::WHITE}));
		pc_keys.emplace_back(std::make_unique<Texture>(game->renderer.get(), &_debug_font, std::string{"K"}, Color{Colors::WHITE}));
	}

	    game->stop_music();
	_dev_button_texture.blend_mode = 1;
	//std::cout << "wow" << std::endl;
	if (game->cfg->god_mode)
	{
		score.is_auto = true;
	}
	    game->ad_manager.load_big_ad();
	    game->ad_manager.load_rewarded_ad();

	    if (game_->cfg->show_banner_ads)
    {
        game_->ad_manager.show_banner();
    }
	score_tps.scale = 0.5L;
	score_tps.offset.y = 0.2L;
	score_tps.show_tps = true;
	/*std::vector<ComposerInfo> composers{
		ComposerInfo{"Franz Liszt", "Hungarian composer, pianist and teacher of the Romantic era", 1811, 1886},
		ComposerInfo{"Frédéric Chopin", "Polish composer and virtuoso pianist of the Romantic period", 1810, 1849},
		ComposerInfo{"Hermann Necke", "German composer, conductor, music director, pianist, and violinist of the Romantic period", 1850, 1912},
		ComposerInfo{"Johann Pachelbel", "German composer, organist, and teacher who brought the south German organ schools to their peak", 1653, 1706},
		ComposerInfo{"Pyotr Ilyich Tchaikovsky", "Russian composer of the Romantic period", 1840, 1893},
		ComposerInfo{"Ludwig van Beethoven", "German composer and pianist", 1770, 1827},
		ComposerInfo{"Edvard Grieg", "Norwegian composer and pianist, he is widely considered one of the main Romantic era composers", 1843, 1907},
		ComposerInfo{"Erik Satie", "French composer and pianist", 1866, 1925},																				//7
		ComposerInfo{"Euphemia Allen", "British composer (under the pen name Arthur de Lulli)", 1861, 1949},
		ComposerInfo{"Wolfgang Amadeus Mozart", "prolific and influential composer of the Classical period", 1756, 1791},
		ComposerInfo{"Camille Saint-Saëns", "French composer, organist, conductor and pianist of the Romantic era", 1835, 1921},							//10
		ComposerInfo{"Johannes Brahms", "German composer, pianist, and conductor of the mid-Romantic period", 1833, 1897},									//11
		ComposerInfo{"Nikolay Nekrasov", "Russian poet, writer, critic and publisher", 1821, 1877},															//12
		ComposerInfo{"Carl Bohm", "German pianist and composer", 1844, 1920},//13
		ComposerInfo{"Robert Schumann", "German composer, pianist and music critic", 1810, 1856},//14
		ComposerInfo{"Amédée Méreaux", "French composer, pianist, piano teacher, musicologist and music critic", 1802, 1874},//15
		ComposerInfo{"Johann Sebastian Bach", "German composer and musician of the late Baroque period", 1685, 1750},//16
	};
	
	std::vector<SongBasicInfo> songs{
		SongBasicInfo{1888, "Anitra's Dance", "", "Anitras Dance", {6}},																					//0
		SongBasicInfo{0, "Canon in D", "accompanied canon by the German Baroque composer Johann Pachelbel", "Canon", {3}},									//1
		SongBasicInfo{0, "Csikós Post", "galop in the key of E minor", "Csikos Post", {2}},
		SongBasicInfo{0, "Danse chinoise", "", "Danse chinoise", {4}},
		SongBasicInfo{0, "Etude in C major", "", "Etude in C major", {0}},
		SongBasicInfo{0, "Etude in d minor", "", "Etude in d minor", {0}},																					//5
		SongBasicInfo{1810, "Für Elise", "one of Ludwig van Beethoven's most popular compositions", "Fur Elise", {5}},
		SongBasicInfo{0, "Prelude Op. 28, No. 4", "by Chopin's request, this piece was played at his own funeral", "Prelude Op 28 No 4", {1}},
		SongBasicInfo{1890, "Gnossienne No. 1", "", "Gnossienne No 1", {7}},
		SongBasicInfo{1890, "Gnossienne No. 2", "", "Gnossienne No 2", {7}},
		SongBasicInfo{1850, "Liebestraum No. 3", "the last of the three that Liszt wrote and the most popular", "Liebestraum No 3", {0}},					//10
		SongBasicInfo{1877, "Chopsticks", "a simple, widely known waltz for the piano", "Chopsticks", {8}},
		SongBasicInfo{1788, "Piano Sonata No. 16", "described by Mozart in his own thematic catalogue as \"for beginners\"", "Piano Sonata No 16", {9}},
		SongBasicInfo{1917, "Sonatine bureaucratique", "the final entry in his humoristic piano music of the 1910s", "Sonatine bureaucratique", {7}, 3, 3},	//13
		SongBasicInfo{1829, "Étude Op. 10, No. 1", "also known as the Waterfall étude", "Etude Op 10 No 1", {1}, 4, 2}, 									//14
		SongBasicInfo{1897, "Petite ouverture à danser", "the piece was one of many Satie left unpublished", "Petite ouverture a danser", {7}, 1, 5}, 		//15
		SongBasicInfo{1867, "Waltz, Op. 12, No. 2", "", "Waltz Op 12 No 2", {6}, 2, 4}, 																	//16

		SongBasicInfo{1882, "Csárdás macabre", "written in a miniature sonata form", "Csardas macabre", {0}, 4, 1, "JKen777"}, //csardas macabre			/17
		SongBasicInfo{1888, "Gymnopédie No. 1", "", "Gymnopedie No 1", {7}, 1, 2, "JKen777"},
		SongBasicInfo{1888, "Gymnopédie No. 2", "its publication was announced in several magazines", "Gymnopedie No 2", {7}, 1, 1, "pszemyslavv_"},

		SongBasicInfo{0, "Hungarian Dance No. 5", "based on the csárdás \"Bártfai emlék\"", "Hungarian Dance No 5", {11}, 3, 1},							//20

		SongBasicInfo{1861, "Korobeiniki", "Nekrasov dedicated it to Gavriil Zakharov", "Korobeiniki", {12}, 2, 1, "NathanTalksTech"},
		SongBasicInfo{1886, "Le cygne", "originally scored for solo cello accompanied by two pianos", "Le cygne", {10}, 1, 3, "NathanTalksTech"},
		SongBasicInfo{1881, "Nuages gris", "one of Liszt's most experimental works", "Nuages gris", {0}, 0, 0, "pszemyslavv_"},
		SongBasicInfo{1839, "Prelude Op. 28, No. 1", "dedicated to Camille Pleyel", "Prelude Op 28 No 1", {1}, 2, 2, "JKen777"},								//24

		SongBasicInfo{0, "The Fountain", "", "The Fountain", {13}, 0, 0, "NathanTalksTech"}	,							//25
		SongBasicInfo{0, "Prélude Opus 28, No. 7 in A Major", "", "Prelude Opus 28 No 7 in A Major", {1}, 0, 0},								//26
		SongBasicInfo{0, "Carnaval Op. 9, No. 12", "", "Carnaval Op 9 No 12", {14}, 0, 0}			,					//27

		SongBasicInfo{0, "Grande Etude Op. 63, No. 24", "", "Etude Op 63 No 24 Bravura", {15}, 0, 0, "JKen777"},								//28
		SongBasicInfo{0, "Prelude in C minor Op. 28, No. 20", "", "Prelude Op 28 No 20", {1}, 0, 0, "JKen777"}	,							//29
		SongBasicInfo{0, "Invention No. 1 in C Major, BWV 772", "", "Invention No 1 in C Major BWV 772", {16}, 0, 0, "JKen777"},								//30
		SongBasicInfo{0, "Invention No. 2 in C Minor, BWV 773", "", "Invention 2 in C Minor BWV 773", {16}, 0, 0, "JKen777"}	,							//31
		SongBasicInfo{0, "Invention No. 3 in D Major, BWV 774", "", "Invention 3 in D Major BWV 774", {16}, 0, 0, "JKen777"}	,							//32
		SongBasicInfo{0, "Wiegenlied Op. 49, No. 4", "", "Wiegenlied Op 49 No 4", {11}, 0, 0, "JKen777"}	,							//33
		SongBasicInfo{0, "Invention No. 4 in D Minor, BWV 775", "", "Invention 4 in D minor BWV 775", {16}, 0, 0, "JKen777"},								//34
		SongBasicInfo{0, "Grande Etude Op. 63, No. 60", "", "Grande Etude Op 63 No 60", {15}, 0, 0, "JKen777"}								//35

	};

	songs.at(4).leaderboard_id = "CgkIze-zr88ZEAIQAQ"; 

	SongDatabase database;

	for (uint16_t i = 0; i < composers.size(); ++i)
	{
		database.composers_infos.emplace_back(i, composers.at(i));
	}

	database.songs_infos = {
		{9, songs.at(9)}, //Gnossienne No. 2
		{1, songs.at(1)},//canon in d
		{7, songs.at(7)},//prelude op 28 no 4
		{11, songs.at(11)},//chopsticks
		{8, songs.at(8)}, //Gnossienne No. 1
		{15, songs.at(15)}, //Petite ouverture a danser 
		{3, songs.at(3)},//danse chinoise
		{16, songs.at(16)}, //Waltz Op 12 No 2
		{6, songs.at(6)},//fur elise
		{13, songs.at(13)}, //Sonatine bureaucratique
		{10, songs.at(10)},//Liebestraum no 3
		{0, songs.at(0)},//anitras dance
		{2, songs.at(2)},//csikos post
		{14, songs.at(14)}, //Etude Op 10 No 1
		{5, songs.at(5)},//etude in d minor
		{12, songs.at(12)},//sonata no 16
		{4, songs.at(4)},//etude in c major

		{17, songs.at(17)},
		{18, songs.at(18)},
		{19, songs.at(19)},
		{20, songs.at(20)},
		{21, songs.at(21)},
		{22, songs.at(22)},
		{23, songs.at(23)},
		{24, songs.at(24)},

		{25, songs.at(25)},
		{26, songs.at(26)},
		{27, songs.at(27)},
		{28, songs.at(28)},
		{29, songs.at(29)},
		{30, songs.at(30)},
		{31, songs.at(31)},
		{32, songs.at(32)},
		{34, songs.at(34)},
		{33, songs.at(33)},
		{35, songs.at(35)},
		};
	auto ofile = open_ofile("songs.db");
	database.to_file(ofile.value());
	ofile->close();
	std::abort();*/

	/*SongList list;
	list.songs.emplace_back("Canon", "Johann Pachelbel", "Canon.usong");
	list.songs.emplace_back("Für Elise", "Ludwig van Beethoven", "Fur Elise.usong");
	list.songs.emplace_back("Anitra's Dance", "Edvard Grieg", "Anitras Dance.usong");
	list.songs.emplace_back("Etude in d minor", "Franz Liszt", "Etude in d minor.usong");
	list.songs.emplace_back("Etude in C major", "Franz Liszt", "Etude in C major.usong");
	list.songs.emplace_back("Csikós Post", "Hermann Necke", "Csikos Post.usong");
	auto ofile = open_ofile("newsonglist.txt");
	list.to_file(ofile.value());
	ofile->flush();
	std::abort();*/
	

	tile_divider.blend_mode = 1;
	txt_arrow->blend_mode = 1;
	txt_single_tile_cleared.blend_mode = 1;
	slider_tile.blend_mode = 1;
	slider_tile_clearing.blend_mode = 1;
	txt_long_tile_end.blend_mode = 1;
	txt_long_tile_circle.blend_mode = 1;
	tile_divider.tint = { 200, 255, 255, 80 };
	if (halloween_theme)
	{
	tile_divider.tint = { 200, 100, 0, 80 };
	}

	std::string soundfont_filename = "";
	if (game->cfg->preferred_soundfont && game->cfg->preferred_soundfont <= extra_soundfonts.size())
	{
		const auto path = Path::user(std::to_string(game->cfg->preferred_soundfont) + ".sf2", "downloaded");
		if (std::filesystem::exists(path))
		{
			std::ifstream file(path, std::ios::binary);
        	std::vector<unsigned char> hash(picosha2::k_digest_size);
        	picosha2::hash256(file, hash.begin(), hash.end());
        	const std::string hex_str = picosha2::bytes_to_hex_string(hash.begin(), hash.end());
        	if (hex_str == extra_soundfonts.at(game->cfg->preferred_soundfont - 1).hash)
			{
				soundfont_filename = path;
			}
		}
	}
	soundfont = game->audio->load_soundfont(soundfont_filename);
	soundfont->set_volume(std::pow(Number(game->cfg->soundfont_volume), 3.0L) / 2097152.0L);
	soundfont->always_sustain = game->cfg->sustain_mode;

	if (t_score_filename.empty())
	{
		std::string filename;
		{
			ExtractedRes song_list_res("songs.db", "database");
			auto song_list_file = open_ifile(song_list_res.get_path()).value();
			SongDatabase song_database = song_list_file;
			//find filename
			for (const auto& [id, info] : song_database.songs_infos)
			{
				if (id == song_id)
				{
					if (!info.leaderboard_id.empty())
					{
						scoreboard_id = info.leaderboard_id;
					}
					filename = info.filename + ".usong";
					if (!info.maker.empty()) {
					Font maker_font{game->renderer.get(), "roboto.ttf", 8.0L};
					lbl_maker.emplace("by " + info.maker, 1.0L, Vec2{0.0L, 0.75L}, Vec2{}, &maker_font, game->renderer.get());
					//lbl_maker->label_text_texture->tint = {255, 64, 64, 255};
					}
					break;
				}
			}
		}
		ExtractedRes song_info_res(filename, "songs");
		auto song_info_file = open_ifile(song_info_res.get_path()).value();
		_song_info = song_info_file;
	}
	else
	{
		if (t_from_res)
		{
			ExtractedRes res(t_score_filename.data(), "songs");
			auto song_info_file = open_ifile(res.get_path()).value();
			_song_info = song_info_file;
		}
		else
		{
			auto song_info_file = open_ifile(t_score_filename.data()).value();
			_song_info = song_info_file;
		}
		save_score = false;
	}

	if (pc_controls &&game->cfg->pc_user)
	{
		for (auto& tile : _song_info.tiles)
		{
			if (tile.type == TileInfo::Type::SLIDER)
			{
				tile.type = TileInfo::Type::LONG;
			}
		}
	}

	if (game->cfg->starting_tps)
	{
		save_score = false;
		if (_song_info.acceleration_method == SongInfo::AccelerationMethod::LINEAR)
		{
			for (auto& [tile_id, tps] : _song_info.acceleration_info.tempo_changes)
			{
				tps *= (game->cfg->starting_tps/Number(_song_info.starting_tempo));
			}
		}
		_song_info.starting_tempo = game->cfg->starting_tps;

		_song_info.acceleration_info.tempo_changes.clear();
	}
	tps = 0.0L;
	_old_tp = new_tp = last_tempo_change = {};
	previous_position = 3.0L;
	_position = 3.0L;
	if (_song_info.acceleration_method == SongInfo::AccelerationMethod::ACCELERATION)
	{
		score.show_tps = true;
	}
	//_song_info.starting_tempo = 10.0L;
	//_song_info.acceleration_method = SongInfo::AccelerationMethod::ACCELERATION;
	//_song_info.acceleration_info.parameter = 3.0L;
	if (false) //double length tiles mode
	{
		_song_info.starting_tempo /= 2.0L;
		_song_info.length_units_per_single_tile *= 2;
		_song_info.note_ticks_per_single_tile *= 2;
		/*for (auto& tile : _song_info.tiles)
		{
					
		}*/
	}

	//_song_info.acceleration_method = SongInfo::AccelerationMethod::CLASSIC;
	//_song_info.acceleration_info.classic_tempo_changes = {std::make_pair<uint32_t, Number>(10, 2.0L), {20, 7.0L}};

	if (true || game->cfg->use_pt2_method) // 3 section mode
	{
		if (_song_info.acceleration_method == SongInfo::AccelerationMethod::LINEAR)
		{
			auto& tc = _song_info.acceleration_info.tempo_changes;
			if (tc.empty() && _song_info.tiles.size() >= 3)
			{
				_song_info.acceleration_method = SongInfo::AccelerationMethod::CLASSIC;
				const std::array<uint32_t, 2> indexes{
					_song_info.tiles.size() / uint64_t{3},
					uint64_t(_song_info.tiles.size()) * uint64_t{2} / uint64_t{3}
				};
				const Number mini_param = std::cbrt(_song_info.acceleration_info.parameter);
				_song_info.acceleration_info.classic_tempo_changes[0]=std::pair<uint32_t, Number>(indexes[0], _song_info.starting_tempo * mini_param);
				_song_info.acceleration_info.classic_tempo_changes[1]=std::pair<uint32_t, Number>(indexes[1], _song_info.starting_tempo * mini_param * mini_param);
			}
		}
	}
	else if (/*game->cfg->three_section_mode*/ true) // 3 section mode
	{
		if (_song_info.acceleration_method == SongInfo::AccelerationMethod::LINEAR)
		{
			auto& tc = _song_info.acceleration_info.tempo_changes;
			if (tc.empty() && _song_info.tiles.size() >= 3)
			{
				const std::array<uint32_t, 2> indexes{
					_song_info.tiles.size() / uint64_t{3},
					uint64_t(_song_info.tiles.size()) * uint64_t{2} / uint64_t{3}
				};
				const Number mini_param = std::cbrt(_song_info.acceleration_info.parameter);
				tc.emplace(indexes[0], _song_info.starting_tempo * mini_param);
				tc.emplace(indexes[1], _song_info.starting_tempo * mini_param * mini_param);
			}
		}
	}

	if (game->cfg->show_tps_instead_of_score) //always show TPS mode
	{
		score.show_tps = true;
	}

	if (game->cfg->limit_note_velocity)
	{
		for (auto& tile : _song_info.tiles)
		{
			for (auto& [delta, event] : tile.note_events)
			{
				if (event.type == NoteEvent::Type::ON)
				{
					event.velocity = std::max(std::uint16_t(event.velocity) * 72 / 127, 1);
				}
			}
			if (tile.type == TileInfo::Type::DOUBLE)
			{
				for (auto& [delta, event] : tile.note_events_2nd_tile)
				{
					if (event.type == NoteEvent::Type::ON)
					{
						event.velocity = std::max(std::uint16_t(event.velocity) * 72 / 127, 1);
					}
				}
			}
		}
	}

	txt_white.blend_mode = 1;
	tp_state_start={};
	//set_theme_tint(theme_tint);
	//_song_info.starting_tempo *= 3.0L;
	txt_trail->blend_mode = 1;
}

void StateLevel::render_debug() const
{
	constexpr std::chrono::steady_clock::period steady_p;
	constexpr std::chrono::system_clock::period system_p;
	constexpr std::chrono::high_resolution_clock::period high_p;
	static const std::string steady_s = std::to_string(steady_p.num) + "/" + std::to_string(steady_p.den);
	static const std::string system_s = std::to_string(system_p.num) + "/" + std::to_string(system_p.den);
	static const std::string high_s = std::to_string(high_p.num) + "/" + std::to_string(high_p.den);
	static const std::string all_clocks = steady_s + ";" + system_s + ";" + high_s;

	Texture dbg_tps{ game->renderer.get(), &_debug_font, "TPS: " + std::to_string(tps), Colors::WHITE };
	Texture dbg_tiles_n{ game->renderer.get(), &_debug_font, "Active tiles: " + std::to_string(tiles.size()), Colors::WHITE };
	Texture dbg_cleared_n{ game->renderer.get(), &_debug_font, "Cleared tiles: " + std::to_string(cleared_tiles), Colors::WHITE };
	Texture dbg_position{ game->renderer.get(), &_debug_font, "Position: " + std::to_string(_position), Colors::WHITE };
	Texture dbg_lap{ game->renderer.get(), &_debug_font, "Lap: " + std::to_string(lap_id), Colors::WHITE };
	Texture dbg_notes{ game->renderer.get(), &_debug_font, "Notes playing: " + std::to_string(soundfont->get_notes_on_size()), soundfont->get_notes_on_size() ? Colors::WHITE : Colors::RED };
	Texture dbg_clock{ game->renderer.get(), &_debug_font, "Clock: " + all_clocks, Colors::WHITE };

	game->renderer->render(&dbg_cleared_n, {}, dbg_cleared_n.get_psize(), Vec2(-1, 1), dbg_cleared_n.get_rsize(), {}, { -1,1 });
	game->renderer->render(&dbg_tiles_n, {}, dbg_tiles_n.get_psize(), Vec2(-1, 1 - (dbg_cleared_n.get_rsize().y) * 2.0L), dbg_tiles_n.get_rsize(), {}, { -1,1 });
	game->renderer->render(&dbg_tps, {}, dbg_tps.get_psize(), Vec2(-1, 1 - (dbg_cleared_n.get_rsize().y + dbg_tiles_n.get_rsize().y) * 2.0L), dbg_tps.get_rsize(), {}, { -1,1 });
	game->renderer->render(&dbg_position, {}, dbg_position.get_psize(), Vec2(-1, 1 - (dbg_cleared_n.get_rsize().y + dbg_tiles_n.get_rsize().y + dbg_tps.get_rsize().y) * 2.0L), dbg_position.get_rsize(), {}, { -1,1 });
	game->renderer->render(&dbg_lap, {}, dbg_lap.get_psize(), Vec2(-1, 1 - (dbg_cleared_n.get_rsize().y + dbg_tiles_n.get_rsize().y + dbg_tps.get_rsize().y + dbg_position.get_rsize().y) * 2.0L), dbg_lap.get_rsize(), {}, { -1,1 });
	game->renderer->render(&dbg_notes, {}, dbg_notes.get_psize(), Vec2(-1, 1 - (dbg_cleared_n.get_rsize().y + dbg_tiles_n.get_rsize().y + dbg_tps.get_rsize().y + dbg_position.get_rsize().y + dbg_lap.get_rsize().y) * 2.0L), dbg_notes.get_rsize(), {}, { -1,1 });
	game->renderer->render(&dbg_clock, {}, dbg_clock.get_psize(), Vec2(-1, 1 - (dbg_cleared_n.get_rsize().y + dbg_tiles_n.get_rsize().y + dbg_tps.get_rsize().y + dbg_position.get_rsize().y + dbg_lap.get_rsize().y + dbg_notes.get_rsize().y) * 2.0L), dbg_clock.get_rsize(), {}, { -1,1 });
}

void StateLevel::queue_notes(const std::multimap<uint32_t, NoteEvent>& notes, bool forceplay_old, const std::optional<Timepoint> custom_tp)
{
	if (forceplay_old)
	{
		soundfont->play_all_events();
	}
	for (const auto& [offset, note_event] : notes)
	{
		if (_song_info.acceleration_method == SongInfo::AccelerationMethod::ACCELERATION)
		{
			const auto parameter = _song_info.acceleration_info.parameter;
			const auto start_tps = custom_tp.has_value() ? (tps + parameter * (custom_tp.value() - new_tp)) : tps;
			const auto& timepoint = custom_tp.value_or(new_tp);
			const auto desired_length = Number(offset) / Number(_song_info.note_ticks_per_single_tile);
			const auto offset_s = (std::sqrt(start_tps * start_tps + 2.0L * parameter * desired_length) - start_tps) / parameter;
			soundfont->add_event(timepoint + offset_s, note_event);
		}
		else
		{
			soundfont->add_event(custom_tp.value_or(new_tp) + 
				Number(offset) / Number(_song_info.note_ticks_per_single_tile) / tps, note_event);
		}
	}
}

bool StateLevel::force_first_interaction() const
{
	return _state == State::IDLE;
}

void StateLevel::set_theme_tint(Color t_tint)
{
	txt_single_tile_cleared.tint = t_tint;
	txt_single_tile_cleared.tint.a = 160;
	txt_long_tile.tint = t_tint;
	txt_long_tile_ext.tint = t_tint;
	txt_long_tile_clearing.tint = t_tint;
	txt_long_tile_circle.tint = t_tint;
	txt_long_tile_end.tint = t_tint;
	_burst._texture.tint = t_tint;
}

	Number StateLevel::get_miss_range() const noexcept
	{
		if (pc_controls && game->cfg->pc_user)
		{
			return 2.0L;
		}
		return 0.1L + tps * 0.1L;
	}

void StateLevel::update()
{
	new_tp = Timepoint();

	Number delta_time = new_tp - _old_tp;
	if (delta_time > 1.0L && _state == State::ACTIVE)
	{
		return game->change_state<StateSongMenu>();
	}
	_burst.update(delta_time * tps * 0.25L);
	lv_bg.update(delta_time);
	set_theme_tint(theme_tint);
	_old_tp = new_tp;

	for (auto& touch_event : touch_events)
	{
		const auto column = get_column(touch_event.position.x);
		if (touch_event.type == TouchEvent::Type::DOWN)
		{
			if (dt_column == column)
			{
				touch_event.type = TouchEvent::Type::NONE;
				dt_column.reset();
			}
			else
			{
				dt_column = column;
			}
		}
		else if (touch_event.type == TouchEvent::Type::UP)
		{
			dt_column.reset();
		}
	}

	if (game_over_reset.has_value() && new_tp - game_over_reset.value() > 2.0L)
	{
		if (!game->cfg->god_mode && _song_info.acceleration_method != SongInfo::AccelerationMethod::ACCELERATION && can_be_revived && score.get_score() >= 50 /*&& game->ad_manager.can_show_rewarded_ad()*/)
		{
			//game_over_reset.reset();
			if (!revive_tp.has_value())
			{
				game->play_music();
				revive_tp = new_tp;
				if (!game->ad_manager.can_show_rewarded_ad())
				{
					*revive_tp -= 100.0L;
				} 
			}
			/*soundfont->play_all_events();
			perfect_score = false;
			game_over_tile = nullptr;
			game_over_scroll_to.reset();
			can_be_revived = false;
			game_over_reset.reset();
			_state = State::IDLE;
			for (auto& tile : tiles)
			{
				tile.second->revive();
			}
			for (auto& tile : tiles)
			{
				if (tile.second->is_active())
				{
					_position = tile.first + 3.0L;
					break;
				}
				else
				{
					_position = tile.first + 3.0L + tile.second->get_tile_length();
				}
			}
			for (auto& [position, tile] : tiles)
			{ 
				tile->y_offset = _position - position;
			}*/
		}
		else
		{
			if (game->cfg->show_interstitial_ads && game->ad_manager.can_show_big_ad() && !perfect_score)
			{
				game->stop_music();
				game->ad_manager.show_big_ad();
			}
			return game->change_state<StateSongMenu>();
		}
	}

	//handle revive
	const auto revive=[&]()
	{
		soundfont->play_all_events();
			perfect_score = false;
			game_over_tile = nullptr;
			game_over_scroll_to.reset();
			can_be_revived = false;
			game_over_reset.reset();
			_state = State::IDLE;
			game->stop_music();
			for (auto& tile : tiles)
			{
				tile.second->revive();
			}
			for (auto& tile : tiles)
			{
				if (tile.second->is_active())
				{
					_position = tile.first + 3.0L;
					break;
				}
				else
				{
					_position = tile.first + 3.0L + tile.second->get_tile_length();
				}
			}
			for (auto& [position, tile] : tiles)
			{ 
				tile->y_offset = _position - position;
			}
			revive_tp.reset();
	};

	//handle revive
	if (revive_tp.has_value())
	{
		revive_yes.position.y = -0.15L * game->renderer->get_aspect_ratio();
		revive_no.position.y = 0.15L * game->renderer->get_aspect_ratio();
		const auto revive_seconds = get_revive_seconds();

		if (game->ad_manager.is_rewarded())
		{
			return revive();
		}
		else if (revive_yes.update() || pressed_enter)
		{
			if (revive_seconds) //SHOW AD
			{
				game->stop_music();
				game->ad_manager.show_rewarded_ad();
			}
			else //just revive
			{
				return revive();
			}
		}
		else if (revive_no.update() || pressed_backspace)
		{
			if (game->cfg->show_interstitial_ads && game->ad_manager.can_show_big_ad() && !perfect_score)
			{
				game->stop_music();
				game->ad_manager.show_big_ad();
			}
			return game->change_state<StateSongMenu>();
		}
	}

	if (game_over_scroll_to.has_value() && _state == State::GAME_OVER)
	{
		Number total_scroll_length = previous_position - game_over_scroll_to.value() - 4.0L;
		_position = previous_position - std::clamp((new_tp - game_over_reset.value()) * 4.0L, 0.0L, 1.0L) * total_scroll_length;
		for (auto& [position, tile] : tiles)
		{
			tile->y_offset = _position - position;
		}
		return;
	}

	if (_state == State::ACTIVE)
	{
		if (_song_info.acceleration_method == SongInfo::AccelerationMethod::ACCELERATION)
		{
			//TODO: Is this correct!?
			const auto starting_tempo = _song_info.starting_tempo;
			const auto total_time = (new_tp - last_tempo_change);
			tps = starting_tempo + _song_info.acceleration_info.parameter * total_time;
			_position = previous_position + total_time * (starting_tempo + _song_info.acceleration_info.parameter * 0.5L * total_time);
		}
		else
		{
			_position = previous_position + (new_tp - last_tempo_change) * tps;
		}

	}

	//sort touch down
	/*touch_down_sorted_positions.clear();
	for (const auto& [finger_id, touch_pos] : touch_down)
	{
		touch_down_sorted_positions.emplace_back(finger_id, touch_pos);
	}
	std::sort(touch_down_sorted_positions.begin(), touch_down_sorted_positions.end(),
		[](const std::pair<uint16_t, Vec2>& pos_a, const std::pair<uint16_t, Vec2>& pos_b) { return pos_a.second.y < pos_b.second.y; });
	*/

	//update tiles
	for (const auto& [tile_pos, tile] : tiles)
	{
		//update Y offset
		tile->y_offset = _position - tile_pos;
		if (_state == State::GAME_OVER)
		{
			continue;
		}
		tile->update();
		if (game->cfg->god_mode)
		{
			if (tile->is_active() && tile->y_offset > 3.0L)
			{
				//if (tile->get_info().type != TileInfo::Type::SLIDER)
				{
					if (tile->get_info().type == TileInfo::Type::DOUBLE)
					{
						tile->touch_down(0, {get_column_x_pos(tile->column) + 1.0L, 0.0L});
					}
					LongTile::y_finger_tapped = 0.0L;
					if (tile->get_info().type == TileInfo::Type::SLIDER)
					{
						tile->touch_down(0, {0.0L, 0.0L});
					}
					else
					{
						tile->touch_down(0, {get_column_x_pos(tile->column), 0.0L});
					}
				}
			}
		}
	}

	//update Y offset
	//for (auto& [position, tile] : tiles)
	//{
	//	tile->y_offset = _position - position;
	//}

	if (_state == State::ACTIVE)
	{
		//delete old tiles
		for (auto it = tiles.cbegin(); it != tiles.cend();)
		{
			auto action = it->second->get_action();
			if (action == TileAction::GAME_OVER)
			{
				spawn_new_tiles();
				game_over_scroll_to = it->first;
				return game_over(it->second.get());
			}
			if (action == TileAction::DELETE)
			{
				it = tiles.erase(it);
			}
			else ++it;
		}
	}

	//spawn new tiles
	spawn_new_tiles();
}

void StateLevel::render() const
{
	lv_bg.render();

	//_dustmotes_stars.render();
	//_dustmotes.render();
	//game->renderer->render(&bg_o, {}, bg_o.get_psize(), {}, { 1,1 }, {});
	game->renderer->render(&tile_divider, {}, tile_divider.get_psize(), {}, { 0.01,1 }, {});
	game->renderer->render(&tile_divider, {}, tile_divider.get_psize(), {-0.5,0}, { 0.01,1 }, {});
	game->renderer->render(&tile_divider, {}, tile_divider.get_psize(), {0.5,0}, { 0.01,1 }, {});

	if (_state == State::IDLE)
	{
		//const uint8_t arrow_n = std::clamp<uint8_t>(std::clamp<Number>(12.0L / _song_info.starting_tempo, 2.0L, 3.0L), 2, 3);
		const uint8_t arrow_n = [&](){
			const auto tempo = _song_info.starting_tempo;
			if (tempo > 8.0L)
			{
				return 1;
			}
			if (tempo > 4.0L)
			{
				return 2;
			}
			return 3;
		}();
		const Number offset_a = 1.0L / Number(arrow_n);
		const Number seconds_per_tile = 4.0L / _song_info.starting_tempo / Number(arrow_n);
		Number arrow_offset = (new_tp % seconds_per_tile) / 4.0L * _song_info.starting_tempo;
		for (uint8_t i = 0; i < arrow_n; ++i)
		{
			const Number y_pos_offset = arrow_offset * 2.0L - 1.0L;
			const Number size = (1.0L - y_pos_offset * y_pos_offset * y_pos_offset * y_pos_offset) /  4.0L;
			//const Number size = 0.25L;
			const uint8_t tint_r = std::clamp(arrow_offset * 2.0L * 255.0L, 0.0L, 255.0L);
			const uint8_t tint_g = std::clamp(255.0L * 2.0L - arrow_offset * 2.0L * 255.0L, 0.0L, 255.0L);
			//const uint8_t tint_g2 = std::clamp(255.0L * 1.0L - arrow_offset * 2.0L * 255.0L, 0.0L, 255.0L);
			txt_arrow->tint.r = 0xB0;
			txt_arrow->tint.g = 0xD0;
			txt_arrow->tint.b = 0xFF;
			txt_arrow->tint.a = std::clamp(size * 4.0L * 255.0L, 0.0L, 255.0L);
			/*txt_arrow->tint.b = tint_g;
			txt_arrow->tint.g = tint_g2;
			txt_arrow->tint.r = 0;*/
			game->renderer->render(&txt_arrow.value(), {}, txt_arrow->get_psize(), {0.0L, y_pos_offset}, {size, size*game->renderer->get_aspect_ratio()}, {});
			arrow_offset += offset_a;
		}	
	}

	//render tiles
	for (const auto& [position, tile] : tiles)
	{
		//experimental trail eff
		if (!is_game_over() && txt_trail)
		{
			bool render_trail = true;
			if (tile->get_info().type == TileInfo::Type::SINGLE)
			{
				if (dynamic_cast<SingleTile*>(tile.get())->is_state(&SingleTileCleared))
				{
					render_trail = false;
				}
			}
			/*else if (tile->get_info().type == TileInfo::Type::LONG)
			{
				if (dynamic_cast<LongTile*>(tile.get())->is_state(&LongTileFullyCleared))
				{
					render_trail = false;
				}
			}*/
			else
			{
				render_trail = false;
			}
			if (render_trail)
			{
				Vec2 pos = { get_column_x_pos(tile->column), tile->y_offset / 4.0L * 2.0L - 1.0L - tile->get_height() * 2.0L };
				Texture* texture = &txt_trail.value();
				//texture->tint.a = 128;
				game->renderer->render(texture, {}, texture->get_psize(), pos,
				{ 0.25, get_miss_range() / 4.0L }, {}, { 0,1 });
			}
		}
		tile->render_bg();
		if (!game_over_scroll_to.has_value() || game_over_tile != tile.get() || (new_tp % 0.25) > 0.125)
		{
			tile->render_fg();
		}
	}
	//render score counter
	_burst.render();

	if (_state != State::IDLE || !score.show_tps)
	{
		if (game->cfg->god_mode || _song_info.acceleration_method != SongInfo::AccelerationMethod::ACCELERATION)
		{
			score_tps.render();
		}
		score.render(); 
	}

	if (_state == State::IDLE && lbl_maker && false)
	{
		const Vec2 orig_position = lbl_maker->position;
		lbl_maker->position += Vec2{0.00625L * 0.75L, 0.0125*game->renderer->get_aspect_ratio() * 0.75L};
		//Number angle = (new_tp % (6.28318531L / 16.0L) ) * 16.0L;
		//Number c = std::clamp((-std::sin(angle) + 1.0L) * 127.5L, 0.0L, 255.0L);
		lbl_maker->label_text_texture->tint = Colors::BLACK;
		lbl_maker->render(game->renderer.get());
		lbl_maker->position = orig_position;
		lbl_maker->label_text_texture->tint = {255, 63, 63, 255};
		lbl_maker->render(game->renderer.get());
	}

	//experimental
	const Number total_time = new_tp - tp_state_start;
	if (total_time < 0.25L) {
		txt_white.tint = {16, 16, 16, uint8_t(std::clamp((0.25L - total_time) * 255.0L * 4.0L, 0.0L, 255.0L))};
		game->renderer->render(&txt_white, {}, txt_white.get_psize(), {}, {1.0L, 1.0L}, {}); }
	//experimental

	if (game->cfg->show_gameplay_debug_info) render_debug();

	if (revive_tp.has_value())
	{
		txt_white.tint = Colors::WHITE;
		game->renderer->render(&txt_white, {}, txt_white.get_psize(), {}, {1.0L, 0.5L *game->renderer->get_aspect_ratio()}, {}); 

		const auto revive_seconds = get_revive_seconds();

		Texture txt_top(game->renderer.get(), &_debug_font, revive_seconds ? "Revive for ad?" : "Revive for free?", Colors::BLACK);
		game->renderer->render(&txt_top, {}, txt_top.get_psize(), Vec2{0.0L, -0.4L * game->renderer->get_aspect_ratio()}, txt_top.get_rsize(), {});

		if (revive_seconds)
		{
			Texture txt_bottom(game->renderer.get(), &_debug_font, "...or you can wait "+std::to_string(*revive_seconds)+" seconds to revive without ad!", Colors::GRAY);
			game->renderer->render(&txt_bottom, {}, txt_bottom.get_psize(), Vec2{0.0L, 0.4L * game->renderer->get_aspect_ratio()}, txt_bottom.get_rsize() * 0.7L, {});

		}
		revive_yes.render();
		revive_no.render();
	}

	if (pc_controls &&game->cfg->pc_user)
	{
		game->renderer->render(pc_keys[0].get(), {}, pc_keys[0]->get_psize(), Vec2{-0.75L, 0.25L}, pc_keys[0]->get_rsize() * 1.25L, {}); 
		game->renderer->render(pc_keys[1].get(), {}, pc_keys[1]->get_psize(), Vec2{-0.25L, 0.25L}, pc_keys[1]->get_rsize() * 1.25L, {}); 
		game->renderer->render(pc_keys[2].get(), {}, pc_keys[2]->get_psize(), Vec2{0.25L, 0.25L}, pc_keys[2]->get_rsize() * 1.25L, {}); 
		game->renderer->render(pc_keys[3].get(), {}, pc_keys[3]->get_psize(), Vec2{0.75L, 0.25L}, pc_keys[3]->get_rsize() * 1.25L, {}); 
	}
}

Number StateLevel::get_tile_pos(const Tile* tile_)
{
	for (const auto& [position, tile] : tiles)
	{
		if (tile.get() == tile_)
		{
			return position;
		}
	}
	return 0;
}

void StateLevel::restart_level()
{
	previous_tile.reset();
	cleared_tiles = 0;
	score.set(0);
	_state = State::IDLE;
	previous_position = 0;
	tiles.clear();
	spawned_tiles = 0;
	_position = 0;
	game_over_reset.reset();
	tps = _song_info.starting_tempo;
	soundfont->reset();
	game_over_scroll_to.reset();
	game_over_tile = nullptr;
	_old_tp = new_tp = last_tempo_change = {};
}

void StateLevel::spawn_new_tiles()
{
	/*if (spawned_tiles == _song_info.tiles.size())
	{
		spawned_tiles = 0;
	}*/
	for (uint32_t i = spawned_tiles; true; ++i)
	{
		if (spawned_tiles == _song_info.tiles.size())
		{
			spawned_tiles = 0;
			i = 0;
		}
		Number total_pos = Number(total_length) / Number(_song_info.length_units_per_single_tile);
		if (_position > total_pos)
		{
			//std::cout << "Spawning new tile at position " << total_pos << std::endl;
			//TODO: Add checking for not inserted tiles
			//std::cout << "tile index is " << i << std::endl;
			//std::cout << "tile length is " << _song_info.tiles[i].length << std::endl;
			total_length += _song_info.tiles[i].length;
			//std::cout << "total length is " << total_length << std::endl;
			switch (_song_info.tiles[i].type)
			{
			case TileInfo::Type::SINGLE:
				previous_tile = tiles.emplace_back(total_pos, std::make_shared<SingleTile>(this)).second;
				break;
			case TileInfo::Type::LONG:
				previous_tile = tiles.emplace_back(total_pos, std::make_shared<LongTile>(this)).second;
				break;
			case TileInfo::Type::DOUBLE:
				previous_tile = tiles.emplace_back(total_pos, std::make_shared<DoubleTile>(this)).second;
				break;
			case TileInfo::Type::EMPTY:
				previous_tile = tiles.emplace_back(total_pos, std::make_shared<EmptyTile>(this)).second;
				break;
			case TileInfo::Type::SLIDER:
				previous_tile = tiles.emplace_back(total_pos, std::make_shared<SliderTile>(this)).second;
				break;
			case TileInfo::Type::COMBO:
				previous_tile = tiles.emplace_back(total_pos, std::make_shared<ComboTile>(this)).second;
				break;
			default: abort(); break;
			}
			previous_tile->y_offset = _position - total_pos;
			++spawned_tiles;
		}
		else break;
	}
}

void StateLevel::change_tempo(Number new_tps, const Timepoint& tp_now, Number position)
{
	if (tps == new_tps && _state == State::ACTIVE)
		return;
	//std::cout << "Changing tempo from " << tps << " TPS to " << new_tps << " TPS" << std::endl;
	previous_position = position;
	tps = new_tps;
	last_tempo_change = tp_now;
	_state = State::ACTIVE;
	//txt_arrow.reset();
	lbl_maker.reset();
}

void StateLevel::game_over(Tile* tile)
{
	score.silent_update();
	game_over_tile = tile;
	//change_tempo(0, new_tp, _position);
	previous_position = _position;
	_state = State::GAME_OVER; 
	soundfont->play_all_events();
	soundfont->add_event(new_tp, NoteEvent(NoteEvent::Type::ALL_OFF));
	const std::uint16_t game_over_velocity = game->cfg->limit_note_velocity ? 72 : 127;
	soundfont->add_event(new_tp, NoteEvent(NoteEvent::Type::ON, 48, game_over_velocity));
	soundfont->add_event(new_tp, NoteEvent(NoteEvent::Type::ON, 52, game_over_velocity));
	soundfont->add_event(new_tp, NoteEvent(NoteEvent::Type::ON, 55, game_over_velocity));
	soundfont->add_event(new_tp + 2.0L, NoteEvent(NoteEvent::Type::ALL_OFF));
	game_over_reset = new_tp;

	if (game->cfg->god_mode == false && save_score && game->cfg->use_pt2_method == false)
	{
		SongUserDatabase user_database;
		user_database.load_from_files();
		auto score2 = score.get_score();
		if (_song_info.acceleration_method == SongInfo::AccelerationMethod::ACCELERATION)
		{
			score2 = static_cast<decltype(score2)>(tps * 100.0F);
		}
		user_database.update_score(song_id, lap_id, score2, perfect_score);
		if (!scoreboard_id.empty())
		{
			submit_score(scoreboard_id, score2); 
		}
	}
}

ScoreCounter::ScoreCounter(StateLevel* level_, uint32_t init_value)
	:_font{ level_->game->renderer.get(), "roboto.ttf", 16.5L },
	_level{ level_ }
{
	set(init_value);
}

void ScoreCounter::render() const
{
	const std::uint32_t v = show_tps ? 100 : _value;
	if (show_tps)
	{
		std::string my_tps = std::to_string(_level->tps);
		if (my_tps.size() > 5)
		{
			my_tps = my_tps.substr(0, 5);
		}
		if (!old_tps.empty() && !my_tps.empty() && old_tps[0] != my_tps[0])
		{
			_odd += 2;
			_tp_update = _level->new_tp;
		}
		old_tps = my_tps;
		_texture = std::make_unique<Texture>(_level->game->renderer.get(), &_font, my_tps, _level->game->cfg->starting_tps ? Color{ 255, 255, 63, 255 } : Color{ 255, 63, 63, 255 });
		//return;
	}
	else if (is_auto)
	{
		_texture = std::make_unique<Texture>(_level->game->renderer.get(), &_font, "AUTO", Color{ 63, 255, 63, 255 });
	}
	if (is_auto && _level->_song_info.acceleration_method == SongInfo::AccelerationMethod::ACCELERATION)
	{
		_texture = std::make_unique<Texture>(_level->game->renderer.get(), &_font, "AUTO", Color{ 63, 255, 63, 255 });
	}

	Number diff = std::clamp(0.1L - (_level->new_tp - _tp_update), 0.0L, 0.1L);
	diff = diff * diff * 10.0L;
	diff = diff * 2.0L + 0.8L;
	//Number scale = std::clamp(1.0L - (Timepoint() - _tp_update), 0.8L, 1.0L);
	_texture->tint = {64,64,64,255};
	auto angle = (diff - 0.8L) * 32.0L * (((_odd / 2) % 2 == 0) ? -1.0L : 1.0L);
	if (v >= 100)
	{
		angle *= 0.5L;
	}
	if (v >= 10000)
	{
		angle *= 0.5L;
	}
	if (v >= 10000000 || is_auto)
	{
		angle = 0.0L;
	}
	if (is_auto)
	{
		diff = 0.8L;
	}
	_level->game->renderer->render(_texture.get(), { 0,0 }, _texture->get_psize(), Vec2{offset.x, offset.y * _level->game->renderer->get_aspect_ratio()} + Vec2{ 0.00625 * scale,-0.84+0.0125*_level->game->renderer->get_aspect_ratio() * scale},
		Vec2{ _texture->get_rsize().x * 0.8L,_texture->get_rsize().y*diff } * scale, { 0,-0.84 }, {}, angle);
	_texture->tint = {255,255,255,255};
	_level->game->renderer->render(_texture.get(), { 0,0 }, _texture->get_psize(),  Vec2{offset.x, offset.y * _level->game->renderer->get_aspect_ratio()} +Vec2{ 0,-0.84 },
		Vec2{ _texture->get_rsize().x * 0.8L,_texture->get_rsize().y*diff } * scale, { 0.00625,-0.84+0.0125*_level->game->renderer->get_aspect_ratio() }, {}, angle);
}

void ScoreCounter::set(uint32_t value)
{
	_value = value;
	if (show_tps)
	{
		return;
	}
	_texture = std::make_unique<Texture>(_level->game->renderer.get(), &_font, std::to_string(_value), Color{ 255, 63, 63, 255 });
	_tp_update = _level->new_tp;
	_odd++;
}

void ScoreCounter::add(uint32_t value)
{
	set(_value + value);
	was_silent_added = false;
}

void ScoreCounter::silent_add(uint32_t value)
{
	_value += value;
	was_silent_added = true;
}

void ScoreCounter::silent_update()
{
	if (was_silent_added)
	{
		add(0);
	}
	was_silent_added = false;
}
