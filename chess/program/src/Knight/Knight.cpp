#include "Knight.h"
#include "..\Board\Board.h"

Knight::Knight(const std::string& texture_path, Board* board_ptr,
	uint16_t size, bool is_white_flag)
	: Piece(texture_path, board_ptr, size, static_cast<pt::PieceColor>(1 - is_white_flag)),
	directions{}
{}


const std::vector<sf::Vector2i>& 
Knight::GetActiveFields(
	const pt::board_grid_t& gboard,
	sf::Vector2i pos, sf::Vector2i, sf::Vector2i king_pos,
	const pt::PieceList& gpiece_list,
	bool consider_check, bool clear) {

	if (clear) {
		avaible_fields.clear();
	}

	is_check = consider_check;
	directions = { 1, 2 };
	brd_king_pos = king_pos;

	// generate 4 possible moves twice
	for (int i = 0; i < 2; i++) {
		AvaibleMovesCaptures(gboard, gpiece_list, pos);

		directions.d_y = -directions.d_y;
		AvaibleMovesCaptures(gboard, gpiece_list, pos);

		directions.d_x = -directions.d_x;
		AvaibleMovesCaptures(gboard, gpiece_list, pos);

		directions.d_y = -directions.d_y;
		AvaibleMovesCaptures(gboard, gpiece_list, pos);
		
		std::swap(directions.d_x, directions.d_y);
	}

	return avaible_fields;
}


void Knight::AvaibleMovesCaptures(
	const pt::board_grid_t& gboard,
	const pt::PieceList& gpiece_list,
	sf::Vector2i pos) noexcept {

	sf::Vector2i temp_vec(pos.x + directions.d_x, pos.y + directions.d_y);

	if (CheckMoveCaptureField(gboard, temp_vec) and
		(!is_check or CheckCheckSafe(gboard, gpiece_list, pos, temp_vec))) {
		avaible_fields.push_back(temp_vec);
	}
}


inline bool Knight::CheckMoveCaptureField(
	const pt::board_grid_t& gboard, sf::Vector2i pos) noexcept {
	return Board::isValidField(pos) and
	(pt::CheckType(gboard[pos.y][pos.x], pt::BIT_EMPTY) or
		!pt::CheckColor(gboard[pos.y][pos.x], pt::CastToBitColor(piece_color)));
}