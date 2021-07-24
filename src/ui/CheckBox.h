#pragma once

#include "../NonCopyable.h"
#include "../Types.h"
#include "../GameState.h"
#include "../Texture.h"
#include "../Font.h"
#include "Label.h"

class CheckBox //: NonCopyable
{
public:
	CheckBox(Texture* txt_u, Texture* txt_c, Font* font_, const std::string& label_text, Renderer* renderer_, Vec2 position_ = {}, bool is_checked_ = false);

	mutable Label label;

	[[nodiscard]] bool update(GameState* state);
	[[nodiscard]] bool is_checked() const;

	void render(Renderer* renderer) const;

	bool _checked;

private:
	Texture* _txt_c;
	Texture* _txt_u;

	
};