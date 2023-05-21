#pragma once

#include "..\Board\Board.h"

class Game {
public:
	Game(const sf::VideoMode& window_video_mode, const sf::String& window_title,
		const sf::Uint32& style = sf::Style::Default ^ sf::Style::Resize);

	void RunGame();
private:
	sf::RenderWindow window;
	Board board;
};

