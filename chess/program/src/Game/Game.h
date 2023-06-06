#pragma once

#include "..\Board\Board.h"
#include "Menu.h"

#include <thread>
#include <chrono>

class Game {
public:
	Game(const sf::VideoMode& window_video_mode, const sf::String& window_title,
		const bool& show_console_board = false, const sf::Uint32& style = sf::Style::Default ^ sf::Style::Resize);

	// start game process - main game loop
	void RunGame();
private:
	// check given events
	void CheckEvents(const sf::Event::EventType& type);

	// process mouse press
	void CheckPressedMouse();

	sf::RenderWindow window;
	sf::Image window_image;
	Board board;
	Menu menu;

	sf::Vector2i mouse_pos;
};

