#include "Game.h"

// path for pieces images: "./resource/wpawn.png" (example)
Game::Game(const sf::VideoMode& window_video_mode, const sf::String& window_title,
	const sf::Uint32& style)
	: window(window_video_mode, window_title, style), board(window_video_mode.width) {

}

void Game::RunGame() {
	sf::Event processed_event;

	while (window.isOpen()) {
		window.draw(board.GetBoardSprite());
		window.display();

		while (window.waitEvent(processed_event)) {
			if (processed_event.type == sf::Event::Closed) {
				window.close();
			}
		}
	}
}