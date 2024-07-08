#include "ComboTile.hpp"

#include "../StateLevel.h"
#include "../Game.h"

#include <atomic>
#include <cmath>
#include <algorithm>
#include <iostream>

	ComboTile::ComboTile(class StateLevel* level_)
    	:StatableTile(level_, CB_CENTER)

    {

    }

	void ComboTile::revive()
    {
        if (is_state(&ComboTileClearing))
        {
            force_change_state(&ComboTileCleared);
        }
    }

	[[nodiscard]] bool ComboTile::should_game_over() const
    {
        return y_offset > get_tile_length() + 4.0L + _level->get_miss_range();
    }
	[[nodiscard]] bool ComboTile::should_die() const
    {
        return should_game_over();
    }
	void ComboTile::my_update()
    {

    }
	bool ComboTile::touch_down(uint16_t finger_id, Vec2 pos)
    {

    }
	void ComboTile::touch_up(  uint16_t finger_id, Vec2 pos)
    {

    }
	void ComboTile::on_changed_state()
    {

    }

	void ComboTile::render_fg() const
    {

    }