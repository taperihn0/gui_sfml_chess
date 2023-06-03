#include "Game.h"

Game::Game(const sf::VideoMode& window_video_mode, const sf::String& window_title,
	const bool& show_console_board, const sf::Uint32& style)
	: window(window_video_mode, window_title, style), board(window_video_mode.width, show_console_board)
{}

// start game process - main game loop
void Game::RunGame() {
	sf::Event processed_event;
	board.PreparePiecesTemplate();
	board.PrepareBoard();

	while (window.isOpen()) {
		window.draw(board.GetBoardSprite());
		window.display();

		if (window.waitEvent(processed_event)) {
			CheckEvents(processed_event.type);
		}
	}
}

// check given events
void Game::CheckEvents(const sf::Event::EventType& type) {
	switch (type) {
	case sf::Event::Closed: {
		window.close();
		break;
	}
	case sf::Event::KeyPressed: {
		if (sf::Keyboard::Q) {
			window.close();
		}
		break;
	}
	case sf::Event::MouseButtonPressed: {
		CheckPressedMouse();
		break;
	}
	default: {
		break;
	}
	}
}

// process mouse press
void Game::CheckPressedMouse() {
	mouse_pos = sf::Mouse::getPosition(window);
	board.ProcessPressedMouse(mouse_pos);
}