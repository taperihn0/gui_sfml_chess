#include "Bishop.h"
#include "..\Board\Board.h"

Bishop::Bishop(const std::string& texture_path, Board* board_ptr,
	const uint16_t& size, const bool& is_white_flag)
	: Piece(texture_path, board_ptr, size, PieceFlags::PieceColor(2 - is_white_flag))
{}

// return avaible fields of bishop - just diagonals fields
std::vector<sf::Vector2i>&& Bishop::GetActiveFields(
	const std::array<std::array<PieceFlags::Indicator, 8>, 8>& pieces_indicator,
	const sf::Vector2i& pos, bool consider_check, const bool& clear) {
	if (clear) {
		avaible_fields.clear();
	}

	is_check = consider_check;

	// go through diagonals and break when another piece
	// appear or when end of board
	for (const auto& d_y : { -1, 1 }) {
		for (const auto& d_x : { -1, 1 }) {
			ProcessDiagonal(pieces_indicator, pos, sf::Vector2i(d_x, d_y));
		}
	}

	return std::move(avaible_fields);
}

// go through diagonal by doing some calculations on a given direction vector
void Bishop::ProcessDiagonal(
	const std::array<std::array<PieceFlags::Indicator, 8>, 8>& pieces_indicator,
	sf::Vector2i pos, const sf::Vector2i& direction) noexcept {
	
	sf::Vector2i temp_vec(pos + direction);

	while (CheckFieldFreeValid(pieces_indicator, temp_vec) and 
		(!is_check or CheckCheckSafe(pieces_indicator, pos, temp_vec))) {
		avaible_fields.push_back(temp_vec);
		temp_vec += direction;
	}

	if (CheckFieldOccupied(pieces_indicator, temp_vec, piece_color) and
		(!is_check or CheckCheckSafe(pieces_indicator, pos, temp_vec))) {
		avaible_fields.push_back(temp_vec);
	}
}