#pragma once

#include "..\Board\Board.h"
#include "Menu.h"

class Game {
public:
	Game(
		const sf::VideoMode& window_video_mode, const sf::String& window_title,
		const sf::Uint32& style = sf::Style::Default ^ sf::Style::Resize);

	// start game process - main game loop
	void RunGame();
private:
	// check events during displaying starting window
	void CheckEventsStart(const sf::Event& event, bool& run_state);

	// check given events during executing main loop
	void CheckEventsMain(const sf::Event& event);

	void CheckEventsEnd(const sf::Event& event);

	// process mouse press
	void BoardPressedMouse();

	void OpenGameStartWindow();
	void OpenGameEndWindow();

	sf::RenderWindow window;
	sf::Image window_image;
	Board board;
	Menu menu;

	sf::Vector2i mouse_pos;
	bool run_status;
};

