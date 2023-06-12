#include "Game.h"

Game::Game(
	const sf::VideoMode& window_video_mode, const sf::String& window_title,
	const sf::Uint32& style)
	: window(window_video_mode, window_title, style),
	board(window_video_mode.width),
	menu(window_video_mode.width),
	run_status(false)
{}


void Game::RunGame() {
	if (!window_image.loadFromFile("./program/resource/images/king_window_icon.png")) {
		exit(3);
	}
	window.setIcon(window_image.getSize().x, window_image.getSize().y, window_image.getPixelsPtr());

	menu.PrepareMenuWindow();
	menu.PrepareEndWindow();

	while (window.isOpen()) {
		board.PrepareBoard();
		OpenGameStartWindow();

		sf::Event p_event;
		while (run_status) {
			window.draw(board.GetBoardSprite());
			window.display();

			if (board.CheckForMateStealMate()) {
				OpenGameEndWindow();
			}
			else if (window.waitEvent(p_event)) {
				CheckEventsMain(p_event);
			}
		}
	}
}


void Game::CheckEventsStart(const sf::Event& event, bool& run_state) {
	switch (event.type) {
	case sf::Event::Closed: {
		window.close();
		run_state = false;
		break;
	}
	case sf::Event::KeyPressed: {
		if (sf::Keyboard::Q) {
			window.close();
			run_state = false;
		}
		break;
	}
	case sf::Event::MouseMoved: {
		menu.ProcessStartMousePos(event.mouseMove, run_state);
		break;
	}
	case sf::Event::MouseButtonPressed: {
		auto res(menu.ProcessStartMousePress(sf::Mouse::getPosition(window), run_state));

		if (!res) {
			run_state = false;
			run_status = false;
			window.close();
		}

		break;
	}
	default: {
		break;
	}
	}
}


void Game::CheckEventsMain(const sf::Event& event) {
	switch (event.type) {
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
		BoardPressedMouse();
		break;
	}
	default: {
		break;
	}
	}
}


void Game::CheckEventsEnd(const sf::Event& event) {
	switch (event.type) {
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
	case sf::Event::MouseMoved: {
		menu.ProcessEndMousePos(event.mouseMove);
		break;
	}
	case sf::Event::MouseButtonPressed: {
		menu.ProcessEndMousePress(sf::Mouse::getPosition(window), run_status);
		break;
	}
	default: {
		break;
	}
	}
}


void Game::BoardPressedMouse() {
	mouse_pos = sf::Mouse::getPosition(window);
	board.ProcessPressedMouse(mouse_pos);
}


void Game::OpenGameStartWindow() {
	bool start_status(true);
	run_status = true;

	sf::Event p_event;
	while (start_status) {
		window.draw(board.GetBoardSprite());
		window.draw(menu.GetMenuWindowSprite());
		window.display();

		if (window.waitEvent(p_event)) {
			CheckEventsStart(p_event, start_status);
		}
	}

	menu.ResetCurrent();
}


void Game::OpenGameEndWindow() {
	sf::Event p_event;
	
	while (run_status) {
		window.draw(board.GetBoardSprite());
		window.draw(menu.GetEndWindowSprite());
		window.display();

		if (window.waitEvent(p_event)) {
			CheckEventsEnd(p_event);
		}
	}

	menu.ResetCurrent();
}