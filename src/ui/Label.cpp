#include "Label.h"

Label::Label(const std::string& label_text, Number max_width_, Vec2 position_, Vec2 origin_, Font* font, Renderer* renderer_)
	:label_text_texture{std::make_shared<Texture>(renderer_, font, label_text, Colors::WHITE)},
	origin{origin_},
	max_width{max_width_},
	position{position_}
{
}

void Label::render(Renderer* renderer) const
{
	const auto rsize = label_text_texture->get_rsize();
	const auto pos = position;
	if (rsize.x > max_width)
	{
		//TODO: Render scaled down text in a proper way
		renderer->render(label_text_texture.get(), {}, label_text_texture->get_psize(), position, {max_width, (max_width / rsize.x) * rsize.y}, {}, origin);
	}
	else
	{
		renderer->render(label_text_texture.get(), {}, label_text_texture->get_psize(), position, rsize, {}, origin);
	}
}
