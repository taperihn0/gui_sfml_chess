#include "Game.h"

Game::Game(const sf::VideoMode& window_video_mode, const sf::String& window_title,
	const bool& show_console_board, const sf::Uint32& style)
	: window(window_video_mode, window_title, style),
	board(window_video_mode.width, show_console_board),
	menu(window_video_mode.width)
{}


void Game::RunGame() {
	if (!window_image.loadFromFile("./program/resource/images/king_window_icon.png")) {
		exit(3);
	}
	window.setIcon(window_image.getSize().x, window_image.getSize().y, window_image.getPixelsPtr());

	sf::Event processed_event;
	board.PrepareBoard();
	menu.PrepareEndWindow();

	while (window.isOpen()) {
		window.draw(board.GetBoardSprite());
		window.display();

		if (board.CheckForMateStealMate()) {
			window.draw(menu.GetEndWindowSprite());
			window.display();

			std::this_thread::sleep_for(std::chrono::seconds(4));

			window.close();
		}
		else if (window.waitEvent(processed_event)) {
			CheckEvents(processed_event.type);
		}
	}
}


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


void Game::CheckPressedMouse() {
	mouse_pos = sf::Mouse::getPosition(window);
	board.ProcessPressedMouse(mouse_pos);
}