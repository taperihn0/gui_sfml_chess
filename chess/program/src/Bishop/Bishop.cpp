#include "Bishop.h"
#include "..\Board\Board.h"

Bishop::Bishop(const std::string& texture_path, Board* board_ptr,
	uint16_t size, bool is_white_flag)
	: Piece(texture_path, board_ptr, size, static_cast<pt::PieceColor>(1 - is_white_flag))
{}


const std::vector<sf::Vector2i>& 
Bishop::GetActiveFields(
	const pt::board_grid_t& gboard,
	sf::Vector2i pos, sf::Vector2i, sf::Vector2i king_pos,
	const pt::PieceList& gpiece_list,
	bool consider_check, bool clear) {

	if (clear) {
		avaible_fields.clear();
	}

	is_check = consider_check;
	brd_king_pos = king_pos;

	// go through diagonals and break when another piece
	// appear or when end of board
	for (const auto& d_y : { -1, 1 }) {
		for (const auto& d_x : { -1, 1 }) {
			ProcessDiagonal(gboard, gpiece_list, pos, sf::Vector2i(d_x, d_y));
		}
	}

	return avaible_fields;
}


void Bishop::ProcessDiagonal(
	const pt::board_grid_t& gboard,
	const pt::PieceList& gpiece_list,
	sf::Vector2i pos, const sf::Vector2i& direction) noexcept {
	
	sf::Vector2i temp_vec(pos + direction);

	while (CheckFieldFreeValid(gboard, temp_vec)) {
		if (!is_check or CheckCheckSafe(gboard, gpiece_list, pos, temp_vec)) {
			avaible_fields.push_back(temp_vec);
		}

		temp_vec += direction;
	}

	if (CheckFieldOccupied(gboard, temp_vec, piece_color) and
		(!is_check or CheckCheckSafe(gboard, gpiece_list, pos, temp_vec))) {
		avaible_fields.push_back(temp_vec);
	}
}