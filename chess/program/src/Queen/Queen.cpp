#include "Queen.h"
#include "..\Board\Board.h"

Queen::Queen(const std::string& texture_path, Board* board_ptr,
	const uint16_t& size, bool is_white_flag)
	: Piece(texture_path, board_ptr, size, PieceFlags::PieceColor(2 - is_white_flag)),
	Rook(texture_path, board_ptr, size, is_white_flag), 
	Bishop(texture_path, board_ptr, size, is_white_flag)
{}


const std::vector<sf::Vector2i>& Queen::GetActiveFields(
	const PieceFlags::board_grid_t& pieces_indicator,
	const sf::Vector2i& pos, bool consider_check, bool clear) {
	if (clear) {
		avaible_fields.clear();
	}

	Bishop::GetActiveFields(pieces_indicator, pos, consider_check, false);
	Rook::GetActiveFields(pieces_indicator, pos, consider_check, false);

	return avaible_fields;
}