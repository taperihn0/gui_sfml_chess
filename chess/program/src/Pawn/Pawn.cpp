#include "Pawn.h"
#include "..\Board\Board.h"

Pawn::Pawn(const std::string& texture_path, Board* board_ptr,
	const uint16_t& size, const bool& is_white_flag)
	: Piece(texture_path, board_ptr, size, PieceFlags::PieceColor(2 - is_white_flag)),
	direction(is_white_flag? -1 : 1) 
{}

// return active fields of path - its move fields in front 
// of the pawn and diagonal fields if capture is avaible
std::vector<sf::Vector2i>&& Pawn::GetActiveFields(
	const std::array<std::array<PieceFlags::Indicator, 8>, 8>& pieces_indicator,
	const sf::Vector2i& pos, const bool& clear) {
	if (clear) {
		avaible_fields.clear();
	}

	sf::Vector2i temp_vec;

	// append avaible moves of a piece without capturing
	temp_vec.x = pos.x, temp_vec.y = pos.y + direction;
	AvaibleMoves(pieces_indicator, pieces_indicator[pos.y][pos.x].first_move, temp_vec);

	// capturing both diagonals
	temp_vec.x = pos.x - 1, temp_vec.y = pos.y + direction;
	AvaibleCaptures(pieces_indicator, temp_vec);

	return std::move(avaible_fields);
}

// compute avaible moves and append them
void Pawn::AvaibleMoves(const std::array<std::array<PieceFlags::Indicator, 8>, 8>& pieces_indicator,
	const bool& first_move_flag, sf::Vector2i& temp_vec) noexcept {
	if (pieces_indicator[temp_vec.y][temp_vec.x].type == PieceFlags::PieceType::EMPTY) {
		avaible_fields.push_back(temp_vec);

		if (first_move_flag) {
			temp_vec.y += direction;
			avaible_fields.push_back(temp_vec);
		}
	}
}

// append avaible captures for pawn 
void Pawn::AvaibleCaptures(const std::array<std::array<PieceFlags::Indicator, 8>, 8>& pieces_indicator,
	sf::Vector2i& temp_vec) noexcept {
	if (CheckCaptureField(pieces_indicator, temp_vec)) {
		avaible_fields.push_back(temp_vec);
	}

	temp_vec.x += 2;
	if (CheckCaptureField(pieces_indicator, temp_vec)) {
		avaible_fields.push_back(temp_vec);
	}
}

// just check if the field can be captured by the pawn
bool Pawn::CheckCaptureField(
	const std::array<std::array<PieceFlags::Indicator, 8>, 8>& pieces_indicator, 
	const sf::Vector2i& pos) noexcept {
	return Board::isValidField(pos) and
		pieces_indicator[pos.y][pos.x].type != PieceFlags::PieceType::EMPTY and
		pieces_indicator[pos.y][pos.x].color != piece_color;
}