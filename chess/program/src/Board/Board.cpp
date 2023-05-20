#include "Board.h"

Board::Board(const uint16_t& window_size)
	: light_field(sf::Color::Color(196, 164, 132)), dark_field(sf::Color::Color(128, 70, 27)),
	WINDOW_SIZE(window_size), FIELD_SIZE(WINDOW_SIZE / BOARD_SIZE) {
	render_board.create(WINDOW_SIZE, WINDOW_SIZE);
	plain_board.create(WINDOW_SIZE, WINDOW_SIZE);
	PrepareBoard();
}


void Board::PrepareBoard() {
	std::array<sf::Color, 2> grid_colors = {light_field, dark_field};
	sf::RectangleShape field(sf::Vector2f(FIELD_SIZE, FIELD_SIZE));

	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			const sf::Vector2f real_window_pos(float(i * FIELD_SIZE), float(j * FIELD_SIZE));
			fields_coordinates[i][j] = real_window_pos;

			field.setPosition(real_window_pos);
			field.setFillColor(grid_colors[(i + j) % 2]);
			plain_board.draw(field);
		}
	}

	render_board.draw(sf::Sprite(plain_board.getTexture()));
}


void Board::DrawOnField(sf::Sprite& piece_sprite, sf::Vector2i& pos) {
	piece_sprite.setPosition(fields_coordinates[pos.y][pos.x]);
	render_board.draw(piece_sprite);
}


sf::Sprite Board::GetBoardSprite() {
	render_board.display();
	sf::Sprite res(render_board.getTexture());
	
	// preparing for arranging new board
	// in the next move
	render_board.clear();
	render_board.draw(sf::Sprite(plain_board.getTexture()));
	return res;
}


uint16_t Board::GetFieldSize() {
	return FIELD_SIZE;
}