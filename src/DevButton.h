#pragma once

#include "NonCopyable.h"
#include "Types.h"
#include "Texture.h"

#include <string>
#include <optional>
#include <map>
#include <memory>

class GameState;
class Renderer;

class DevButton
{
public:
	Vec2 position;
	Number width;
	const uint32_t id;
	
	DevButton(Vec2 position_, Number width_, uint32_t id_, const std::string& text, Texture* texture, Font* font, GameState* state);

	bool update();
	void render() const;

	void set_text(const std::string& text);

	[[nodiscard]] bool is_pressed() const;
	[[nodiscard]] bool is_held() const;
	[[nodiscard]] bool in_range(Vec2 pos) const;

private:
	GameState* _state;
	Texture* _texture;
	std::unique_ptr<Texture> _text_texture;
	Font* _font;
	std::optional<uint16_t> _held_finger_id;
	bool _pressed = false;
};