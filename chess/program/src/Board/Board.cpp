#include "Board.h"

Board::Board(const uint16_t& window_size)
	: light_field(sf::Color::Color(196, 164, 132)), dark_field(sf::Color::Color(128, 70, 27)),
	highlighted_field(sf::Color::Color(240, 221, 115, 70)), WINDOW_SIZE(window_size), FIELD_SIZE(WINDOW_SIZE / BOARD_SIZE),
	pieces_templates{}, pieces_indicator{}, curr_focused_pos(-1, -1), grid_colors{ light_field, dark_field } {
	render_board.create(WINDOW_SIZE, WINDOW_SIZE);
	plain_board.create(WINDOW_SIZE, WINDOW_SIZE);

	pieces_surface.create(WINDOW_SIZE, WINDOW_SIZE);
	pieces_surface.clear(sf::Color::Color(0, 0, 0, 0));
}

// kind of init function for pieces template
void Board::PreparePiecesTemplate() {
	// pawns templates
	pieces_templates[int(PieceColor::WHITE)][int(PieceType::PAWN)] 
		= std::make_unique<Piece>(Pawn("./program/resource/wpawn.png",
		this, FIELD_SIZE, true));
	pieces_templates[int(PieceColor::BLACK)][int(PieceType::PAWN)] 
		= std::make_unique<Piece>(Pawn("./program/resource/bpawn.png",
		this, FIELD_SIZE, false));

	// rook templates
	pieces_templates[int(PieceColor::WHITE)][int(PieceType::ROOK)] 
		= std::make_unique<Piece>(Rook("./program/resource/wrook.png",
		this, FIELD_SIZE));
	pieces_templates[int(PieceColor::BLACK)][int(PieceType::ROOK)] 
		= std::make_unique<Piece>(Rook("./program/resource/brook.png",
		this, FIELD_SIZE));

	// knight templates
	pieces_templates[int(PieceColor::WHITE)][int(PieceType::KNIGHT)] 
		= std::make_unique<Piece>(Knight("./program/resource/wknight.png",
		this, FIELD_SIZE));
	pieces_templates[int(PieceColor::BLACK)][int(PieceType::KNIGHT)] 
		= std::make_unique<Piece>(Knight("./program/resource/bknight.png",
		this, FIELD_SIZE));

	// bishop templates
	pieces_templates[int(PieceColor::WHITE)][int(PieceType::BISHOP)] 
		= std::make_unique<Piece>(Bishop("./program/resource/wbishop.png",
		this, FIELD_SIZE));
	pieces_templates[int(PieceColor::BLACK)][int(PieceType::BISHOP)] 
		= std::make_unique<Piece>(Bishop("./program/resource/bbishop.png",
		this, FIELD_SIZE));

	// Queen templates
	pieces_templates[int(PieceColor::WHITE)][int(PieceType::QUEEN)] 
		= std::make_unique<Piece>(Queen("./program/resource/wqueen.png",
		this, FIELD_SIZE));
	pieces_templates[int(PieceColor::BLACK)][int(PieceType::QUEEN)] 
		= std::make_unique<Piece>(Queen("./program/resource/bqueen.png",
		this, FIELD_SIZE));

	// and finally king
	pieces_templates[int(PieceColor::WHITE)][int(PieceType::KING)] 
		= std::make_unique<Piece>(King("./program/resource/wking.png",
		this, FIELD_SIZE));
	pieces_templates[int(PieceColor::BLACK)][int(PieceType::KING)] 
		= std::make_unique<Piece>(King("./program/resource/bking.png",
		this, FIELD_SIZE));
}

// arranging starting positions of pieces for both players
void Board::PrepareBoard() {
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

			// drawing default pieces distribution on the transparent
			// pieces surface
			LocatePieceOnSurface(i, j);
		}
	}

	render_board.draw(sf::Sprite(plain_board.getTexture()));
}

// Filling fields with starting postitions of pieces
void Board::InitBoardFields() noexcept {
	const auto&& last_row_index(BOARD_SIZE - 1);

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

// drawing a piece on his special surface
void Board::DrawOnSurfaceField(sf::Sprite& piece_sprite, sf::Vector2f& window_pos) {
	piece_sprite.setPosition(window_pos);
	pieces_surface.draw(piece_sprite);
}

// updating render_board sprite
void Board::UpdateBoard() {
	pieces_surface.display();
	plain_board.display();

	render_board.draw(sf::Sprite(plain_board.getTexture()));
	render_board.draw(sf::Sprite(pieces_surface.getTexture()));
	render_board.display();
}

// function for drawing board on real game window
sf::Sprite Board::GetBoardSprite() {
	UpdateBoard();
	return sf::Sprite(render_board.getTexture());
}

// size in pixel of a single field on board
const uint16_t& Board::GetFieldSize() noexcept {
	return FIELD_SIZE;
}

// process for every single click in the area of game window
void Board::ProcessPressedMouse(const sf::Vector2i &mouse_pos) {
	const sf::Vector2i field_pos((mouse_pos.x - 1) / FIELD_SIZE, 
		(mouse_pos.y - 1) / FIELD_SIZE);

	if (!isValidField(field_pos)) {
		return;
	}

	const auto& picked_piece = pieces_indicator[field_pos.y][field_pos.x];

	// process mouse pressing by focusing a piece,
	// unfocus it or move a piece
	if (!isValidFocused() and
		picked_piece.type != PieceType::EMPTY) {
		FocusPieceField(field_pos);
	}
	else if (field_pos == curr_focused_pos) {
		UnfocusPieceField(field_pos);
	}
}

// for every piece in 2d array
void Board::LocatePieceOnSurface(const uint8_t &row, const uint8_t &col) {
	const auto& piece = pieces_indicator[row][col];

	if (piece.type == PieceType::EMPTY) {
		return;
	}

	pieces_templates[int(piece.color)][int(piece.type)]->DrawPiece(fields_coordinates[row][col]);
}

// focusing after clicking a piece
void Board::FocusPieceField(const sf::Vector2i& field_pos) {
	const auto& window_field_pos = fields_coordinates[field_pos.y][field_pos.x];
	sf::RectangleShape field(sf::Vector2f(FIELD_SIZE, FIELD_SIZE));

	field.setPosition(window_field_pos);
	field.setFillColor(highlighted_field);

	plain_board.draw(field);
	
	// setting focused piece flag
	curr_focused_pos.y = field_pos.y, curr_focused_pos.x = field_pos.x;
}

// unfocusing after clicking exactly focused piece
void Board::UnfocusPieceField(const sf::Vector2i& field_pos) {
	const auto& window_field_pos = fields_coordinates[field_pos.y][field_pos.x];
	sf::RectangleShape field(sf::Vector2f(FIELD_SIZE, FIELD_SIZE));

	field.setPosition(window_field_pos);
	field.setFillColor(grid_colors[!((field_pos.y + field_pos.x) % 2)]);

	plain_board.draw(field);

	// reseting current focused pos
	curr_focused_pos.y = -1, curr_focused_pos.x = -1;
}


bool Board::isValidField(const sf::Vector2i& coords) noexcept {
	return (coords.x >= 0 and coords.x < BOARD_SIZE)
		and (coords.y >= 0 and coords.y < BOARD_SIZE);
}

// checking whether my focus flag is set
bool Board::isValidFocused() noexcept {
	return curr_focused_pos.x != -1 and curr_focused_pos.y != -1;
}