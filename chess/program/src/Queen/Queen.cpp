#include "Queen.h"
#include "..\Board\Board.h"

Queen::Queen(const std::string& texture_path, Board* board_ptr,
	uint16_t size, bool is_white_flag)
	: Piece(texture_path, board_ptr, size, static_cast< pt::PieceColor>(1 - is_white_flag)),
	Rook(texture_path, board_ptr, size, is_white_flag), 
	Bishop(texture_path, board_ptr, size, is_white_flag)
{}


const std::vector<sf::Vector2i>& 
Queen::GetActiveFields(
	const pt::board_grid_t& gboard,
	sf::Vector2i pos, sf::Vector2i, sf::Vector2i king_pos,
	const pt::PieceList& gpiece_list,
	bool consider_check, bool clear) {

	if (clear) {
		avaible_fields.clear();
	}

	Bishop::GetActiveFields(gboard, pos, sf::Vector2i(-1, -1), king_pos, gpiece_list, consider_check, false);
	Rook::GetActiveFields(gboard, pos, sf::Vector2i(-1, -1), king_pos, gpiece_list, consider_check, false);

	return avaible_fields;
}