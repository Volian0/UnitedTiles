#pragma once

#include "../NonCopyable.h"
#include "../Types.h"
#include "../GameState.h"
#include "../Texture.h"
#include "../Font.h"
#include "../Renderer.h"

class Label// : NonCopyable
{
public:
	Label(const std::string& label_text, Number max_width_, Vec2 position_, Vec2 origin_, Font* font, Renderer* renderer_);
	
	Vec2 position;
	const Vec2 origin;
	const Number max_width;
	std::shared_ptr<Texture> label_text_texture;

	void render(Renderer* renderer) const;
};