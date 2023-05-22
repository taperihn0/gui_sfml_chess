#include "Board.h"

Board::Board(const uint16_t& window_size)
	: light_field(sf::Color::Color(196, 164, 132)), dark_field(sf::Color::Color(128, 70, 27)),
	WINDOW_SIZE(window_size), FIELD_SIZE(WINDOW_SIZE / BOARD_SIZE), pieces_templates{}, pieces_indicator{} {
	render_board.create(WINDOW_SIZE, WINDOW_SIZE);
	plain_board.create(WINDOW_SIZE, WINDOW_SIZE);
}

// path for pieces images: "./program/resource/wpawn.png" (example)
void Board::PreparePiecesTemplate() {
	// pawns templates
	pieces_templates[int(PieceColor::WHITE)][int(PieceType::PAWN)] = std::make_unique<Piece>(Pawn("./program/resource/wpawn.png",
		this, FIELD_SIZE, true));
	pieces_templates[int(PieceColor::BLACK)][int(PieceType::PAWN)] = std::make_unique<Piece>(Pawn("./program/resource/bpawn.png",
		this, FIELD_SIZE, false));

	// rook templates
	pieces_templates[int(PieceColor::WHITE)][int(PieceType::ROOK)] = std::make_unique<Piece>(Rook("./program/resource/wrook.png",
		this, FIELD_SIZE));
	pieces_templates[int(PieceColor::BLACK)][int(PieceType::ROOK)] = std::make_unique<Piece>(Rook("./program/resource/brook.png",
		this, FIELD_SIZE));

	// knight templates
	pieces_templates[int(PieceColor::WHITE)][int(PieceType::KNIGHT)] = std::make_unique<Piece>(Knight("./program/resource/wknight.png",
		this, FIELD_SIZE));
	pieces_templates[int(PieceColor::BLACK)][int(PieceType::KNIGHT)] = std::make_unique<Piece>(Knight("./program/resource/bknight.png",
		this, FIELD_SIZE));

	// bishop templates
	pieces_templates[int(PieceColor::WHITE)][int(PieceType::BISHOP)] = std::make_unique<Piece>(Bishop("./program/resource/wbishop.png",
		this, FIELD_SIZE));
	pieces_templates[int(PieceColor::BLACK)][int(PieceType::BISHOP)] = std::make_unique<Piece>(Bishop("./program/resource/bbishop.png",
		this, FIELD_SIZE));

	// Queen templates
	pieces_templates[int(PieceColor::WHITE)][int(PieceType::QUEEN)] = std::make_unique<Piece>(Queen("./program/resource/wqueen.png",
		this, FIELD_SIZE));
	pieces_templates[int(PieceColor::BLACK)][int(PieceType::QUEEN)] = std::make_unique<Piece>(Queen("./program/resource/bqueen.png",
		this, FIELD_SIZE));

	// and finally king
	pieces_templates[int(PieceColor::WHITE)][int(PieceType::KING)] = std::make_unique<Piece>(King("./program/resource/wking.png",
		this, FIELD_SIZE));
	pieces_templates[int(PieceColor::BLACK)][int(PieceType::KING)] = std::make_unique<Piece>(King("./program/resource/bking.png",
		this, FIELD_SIZE));
}


void Board::PrepareBoard() {
	std::array<sf::Color, 2> grid_colors = {light_field, dark_field};
	sf::RectangleShape field(sf::Vector2f(FIELD_SIZE, FIELD_SIZE));
	
	InitBoardFields();

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

	// drawing default pieces distribution on the plain board
	for (uint8_t i = 0; i < BOARD_SIZE; i++) {
		for (uint8_t j = 0; j < BOARD_SIZE; j++) {
			LocatePiece(i, j);
		}
	}
}

// Filling fields with starting postitions of pieces
void Board::InitBoardFields() noexcept {
	const auto&& last_row_index = BOARD_SIZE - 1;

	// rows of pawns
	pieces_indicator[1].fill(Indicator{ PieceColor::BLACK, PieceType::PAWN });
	pieces_indicator[last_row_index - 1].fill(Indicator{ PieceColor::WHITE, PieceType::PAWN });

	// other pieces: rooks
	pieces_indicator[0][0] = Indicator{ PieceColor::BLACK, PieceType::ROOK };
	pieces_indicator[0][last_row_index] = Indicator{ PieceColor::BLACK, PieceType::ROOK };

	pieces_indicator[last_row_index][0] = Indicator{ PieceColor::WHITE, PieceType::ROOK };
	pieces_indicator[last_row_index][last_row_index] = Indicator{ PieceColor::WHITE, PieceType::ROOK };

	// knights
	pieces_indicator[0][1] = Indicator{ PieceColor::BLACK, PieceType::KNIGHT };
	pieces_indicator[0][last_row_index - 1] = Indicator{ PieceColor::BLACK, PieceType::KNIGHT };

	pieces_indicator[last_row_index][1] = Indicator{ PieceColor::WHITE, PieceType::KNIGHT };
	pieces_indicator[last_row_index][last_row_index - 1] = Indicator{ PieceColor::WHITE, PieceType::KNIGHT };

	// bishops
	pieces_indicator[0][2] = Indicator{ PieceColor::BLACK, PieceType::BISHOP };
	pieces_indicator[0][last_row_index - 2] = Indicator{ PieceColor::BLACK, PieceType::BISHOP };

	pieces_indicator[last_row_index][2] = Indicator{ PieceColor::WHITE, PieceType::BISHOP };
	pieces_indicator[last_row_index][last_row_index - 2] = Indicator{ PieceColor::WHITE, PieceType::BISHOP };

	// queens
	pieces_indicator[0][3] = Indicator{ PieceColor::BLACK, PieceType::QUEEN };
	pieces_indicator[last_row_index][3] = Indicator{ PieceColor::WHITE, PieceType::QUEEN };

	// kings
	pieces_indicator[0][4] = Indicator{PieceColor::BLACK, PieceType::KING};
	pieces_indicator[last_row_index][4] = Indicator{ PieceColor::WHITE, PieceType::KING };
}


void Board::DrawOnField(sf::Sprite& piece_sprite, sf::Vector2f& window_pos) {
	piece_sprite.setPosition(window_pos);
	render_board.draw(piece_sprite);
}


sf::Sprite Board::GetBoardSprite() {
	render_board.display();
	return sf::Sprite(render_board.getTexture());
}


const uint16_t& Board::GetFieldSize() noexcept {
	return FIELD_SIZE;
}


void Board::LocatePiece(const uint8_t &row, const uint8_t &col) {
	const auto& piece = pieces_indicator[row][col];

	if (piece.type == PieceType::EMPTY) {
		return;
	}

	pieces_templates[int(piece.color)][int(piece.type)]->DrawPiece(fields_coordinates[row][col]);
}