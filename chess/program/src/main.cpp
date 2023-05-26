#include "Game\Game.h"

int main() {
	Game game_process(sf::VideoMode(800, 800), "chess", false);
	game_process.RunGame();
}