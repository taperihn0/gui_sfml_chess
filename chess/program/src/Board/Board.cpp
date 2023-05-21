#include "Board.h"

Board::Board(const uint16_t& window_size)
	: light_field(sf::Color::Color(196, 164, 132)), dark_field(sf::Color::Color(128, 70, 27)),
	WINDOW_SIZE(window_size), FIELD_SIZE(WINDOW_SIZE / BOARD_SIZE) {
	render_board.create(WINDOW_SIZE, WINDOW_SIZE);
	plain_board.create(WINDOW_SIZE, WINDOW_SIZE);
}

// path for pieces images: "./program/resource/wpawn.png" (example)
void Board::PreparePiecesTemplate() {
	pieces_templates[int(PieceColor::WHITE)][int(PieceType::PAWN)] = std::make_unique<Piece>(Pawn("./program/resource/wpawn.png",
		this, FIELD_SIZE, true));
	pieces_templates[int(PieceColor::BLACK)][int(PieceType::PAWN)] = std::make_unique<Piece>(Pawn("./program/resource/bpawn.png",
		this, FIELD_SIZE, false));
}


void Board::PrepareBoard() {
	std::array<sf::Color, 2> grid_colors = {light_field, dark_field};
	sf::RectangleShape field(sf::Vector2f(FIELD_SIZE, FIELD_SIZE));
	
	// empty rows are filled by default, 
	// and now filling pawn rows
	pieces_indicator[0].fill(Indicator{ PieceColor::BLACK, PieceType::PAWN });
	pieces_indicator[1].fill(Indicator{ PieceColor::BLACK, PieceType::PAWN });
	pieces_indicator[BOARD_SIZE - 1].fill(Indicator{ PieceColor::WHITE, PieceType::PAWN });
	pieces_indicator[BOARD_SIZE - 1 - 1].fill(Indicator{ PieceColor::WHITE, PieceType::PAWN });

	// preparing plain board with fields
	for (uint8_t i = 0; i < BOARD_SIZE; i++) {
		for (uint8_t j = 0; j < BOARD_SIZE; j++) {
			const sf::Vector2f real_window_pos(float(j * FIELD_SIZE), float(i * FIELD_SIZE));

			// setting window position for every field
			// and arranging even-odd fields 
			field.setPosition(real_window_pos);
			field.setFillColor(grid_colors[!((i + j) % 2)]);
			plain_board.draw(field);

			fields_coordinates[i][j] = real_window_pos;
		}
	}

	render_board.draw(sf::Sprite(plain_board.getTexture()));

	// preparing default pieces distribution on the plain board
	for (uint8_t i = 0; i < BOARD_SIZE; i++) {
		for (uint8_t j = 0; j < BOARD_SIZE; j++) {
			LocatePiece(i, j);
		}
	}
}


void Board::DrawOnField(sf::Sprite& piece_sprite, sf::Vector2f& window_pos) {
	piece_sprite.setPosition(window_pos);
	render_board.draw(piece_sprite);
}


sf::Sprite Board::GetBoardSprite() {
	render_board.display();
	return sf::Sprite(render_board.getTexture());
}


uint16_t Board::GetFieldSize() {
	return FIELD_SIZE;
}


void Board::LocatePiece(const uint8_t &row, const uint8_t &col) {
	const auto& piece = pieces_indicator[row][col];

	if (piece.type == PieceType::EMPTY) {
		return;
	}

	pieces_templates[int(piece.color)][int(piece.type)]->DrawPiece(fields_coordinates[row][col]);
}