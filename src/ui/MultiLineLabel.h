#pragma once

#include "../NonCopyable.h"
#include "../Types.h"
#include "../GameState.h"
#include "../Texture.h"
#include "../Font.h"
#include "../Renderer.h"

class MultiLineLabel : NonCopyable
{
public:
	MultiLineLabel(const std::string& label_text, Number max_width_, Vec2 position_, std::shared_ptr<Font> font_, Renderer* renderer_);
	
	Vec2 position;
    const Number height;
	std::vector<std::unique_ptr<Texture>> label_text_texture;
    std::shared_ptr<Font> font;
    //void update(Numbet t_aspect_ratio);

	void render(Renderer* renderer) const;
};