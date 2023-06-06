#pragma once

#include <SFML\Graphics.hpp>

#include <array>

class Menu {
public:
	Menu(uint16_t window_size_);

	// kind of init function for end game window
	void PrepareEndWindow();

	sf::Sprite GetEndWindowSprite();
private:
	// load resources from hard drive, i.e. font and images
	void LoadResources();

	// prepare displayed text on a end game window
	void PrepareText();

	sf::RenderTexture end_window_tex;
	sf::Sprite end_window;

	sf::RectangleShape frame;

	sf::Texture 
		player_icon_tex,
		bot_icon_tex;
	sf::Sprite 
		player_icon,
		bot_icon;

	std::array<sf::Text, 5> text;
	sf::Font italic_f;

	sf::Color menu_color;

	const uint16_t WINDOW_SIZE;
	static constexpr uint16_t 
		END_WINDOW_WIDTH = 300,
		END_WINDOW_HEIGHT = 300;
};

