#include "Rook.h"
#include "..\Board\Board.h"

Rook::Rook(const std::string& texture_path, Board* board_ptr,
	uint16_t size, bool is_white_flag)
	: Piece(texture_path, board_ptr, size, static_cast<pt::PieceColor>(1 - is_white_flag)),
	directions{ { {0, 1}, {0, -1}, {1, 0}, {-1, 0} } }
{}


const std::vector<sf::Vector2i>& 
Rook::GetActiveFields(
	const pt::board_grid_t& gboard,
	sf::Vector2i pos, sf::Vector2i, sf::Vector2i king_pos,
	const pt::PieceList& gpiece_list,
	bool consider_check, bool clear) {

	if (clear) {
		avaible_fields.clear();
	}

	is_check = consider_check;
	brd_king_pos = king_pos;

	for (const auto& direct : directions) {
		ProcessLine(gboard, gpiece_list, pos, sf::Vector2i(direct.d_x, direct.d_y));
	}

	return avaible_fields;
}


void Rook::ProcessLine(
	const pt::board_grid_t& gboard,
	const pt::PieceList& gpiece_list,
	sf::Vector2i pos, sf::Vector2i direction) noexcept {

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