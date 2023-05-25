#include "Knight.h"
#include "..\Board\Board.h"

Knight::Knight(const std::string& texture_path, Board* board_ptr,
	const uint16_t& size, const bool& is_white_flag)
	: Piece(texture_path, board_ptr, size, PieceFlags::PieceColor(is_white_flag + 1)),
	directions{ 2, 1 }
{}

// return current active fields of knight piece
std::vector<sf::Vector2i>&& Knight::GetActiveFields(
	const std::array<std::array<PieceFlags::Indicator, 8>, 8>& pieces_indicator,
	const sf::Vector2i& pos) {
	avaible_fields.clear();

	// (1) arm
	AvaibleMovesCaptures(pieces_indicator);

	// (2) arm
	directions.d_x = -directions.d_x;
	AvaibleMovesCaptures(pieces_indicator);

	// (3) arm
	std::swap(directions.d_x, directions.d_y);
	AvaibleMovesCaptures(pieces_indicator);

	// (4) arm
	directions.d_y= -directions.d_y;
	AvaibleMovesCaptures(pieces_indicator);

	return std::move(avaible_fields);
}

// appending one avaible 'arm' from 4 avaible arms:
// ---| (1)
// |--- (2)
// ...
void Knight::AvaibleMovesCaptures(const std::array<std::array<PieceFlags::Indicator, 8>, 8>& pieces_indicator) {
	sf::Vector2i temp_vec;

	temp_vec.x = directions.d_x, temp_vec.y = directions.d_y;
	if (CheckMoveCaptureField(pieces_indicator, temp_vec)) {
		avaible_fields.push_back(temp_vec);
	}

	temp_vec.y = -temp_vec.y;
	if (CheckMoveCaptureField(pieces_indicator, temp_vec)) {
		avaible_fields.push_back(temp_vec);
	}
}


bool Knight::CheckMoveCaptureField(const std::array<std::array<PieceFlags::Indicator, 8>, 8>& pieces_indicator,
	const sf::Vector2i& pos) noexcept {
	return Board::isValidField(pos) and
		(pieces_indicator[pos.y][pos.x].type == PieceFlags::PieceType::EMPTY or
			pieces_indicator[pos.y][pos.x].color != piece_color);
}