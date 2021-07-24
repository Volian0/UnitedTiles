#pragma once

#include "../NonCopyable.h"
#include "../Types.h"
#include "../GameState.h"
#include "../Texture.h"
#include "../Font.h"
#include "../Renderer.h"
#include "Label.h"

#include <optional>

class Slider : NonCopyable
{
public:
	Slider(const std::string& label_text, Font* font, Texture* t1, Texture* t2, int32_t init_value, int32_t min_, int32_t max_, Renderer* renderer_, uint32_t snap = 1);

	mutable Label label;

	[[nodiscard]] std::optional<int32_t> update(GameState* state);

	void render(Renderer* renderer) const;

	int32_t value;
	const int32_t min;
	const int32_t max;
};

