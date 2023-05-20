#pragma once

#include "..\Board\Board.h"

#include <vector>
#include <iostream>

class Game {
public:
	Game(const sf::VideoMode& window_video_mode, const sf::String& window_title,
		const sf::Uint32& style = sf::Style::Default);

	void RunGame();
private:
	sf::RenderWindow window;
	Board board;
};

