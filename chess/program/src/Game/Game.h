#pragma once

#include "..\Board\Board.h"

class Game {
public:
	Game(const sf::VideoMode& window_video_mode, const sf::String& window_title,
		const bool& show_console_board = false, const sf::Uint32& style = sf::Style::Default ^ sf::Style::Resize);

	void RunGame();
private:
	void CheckEvents(const sf::Event::EventType& type) noexcept;
	void CheckPressedMouse();

	sf::RenderWindow window;
	Board board;

	sf::Vector2i mouse_pos;
};

