#include "Queen.h"

Queen::Queen(const std::string& texture_path, Board* board_ptr,
	const uint16_t& size, const bool& is_white_flag)
	: Piece(texture_path, board_ptr, size, PieceFlags::PieceColor(2 - is_white_flag)),
	Rook(texture_path, board_ptr, size, is_white_flag), 
	Bishop(texture_path, board_ptr, size, is_white_flag)
{}

// return active fields of queen - 
// mix of rook and bishop
std::vector<sf::Vector2i>&& Queen::GetActiveFields(
	const std::array<std::array<PieceFlags::Indicator, 8>, 8>& pieces_indicator,
	const sf::Vector2i& pos, const bool& clear) {
	if (clear) {
		avaible_fields.clear();
	}

	Bishop::GetActiveFields(pieces_indicator, pos, false);
	Rook::GetActiveFields(pieces_indicator, pos, false);

	return std::move(avaible_fields);
}