#include "King.h"
#include "..\Board\Board.h"

King::King(const std::string& texture_path, Board* board_ptr,
	const uint16_t& size, const bool& is_white_flag)
	: Piece(texture_path, board_ptr, size, PieceFlags::PieceColor(2 - is_white_flag)),
	directions{ { {-1, -1}, {0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0} } }
{}

// return active fields of king - all of the surrounding fields, except those 
// controlled by enemy
std::vector<sf::Vector2i>&& King::GetActiveFields(
	const std::array<std::array<PieceFlags::Indicator, 8>, 8>& pieces_indicator,
	const sf::Vector2i& pos, bool consider_mate, const bool& clear) {
	if (clear) {
		avaible_fields.clear();
	}

	for (const auto& direct : directions) {
		const auto& new_pos(pos + sf::Vector2i(direct.d_x, direct.d_y));
	
		if (CheckFieldMateSafeValid(pieces_indicator, new_pos)) {
			avaible_fields.push_back(new_pos);
		}
	}

	return std::move(avaible_fields);
}

// check whether field is safe for king
bool King::CheckFieldMateSafeValid(
	const std::array<std::array<PieceFlags::Indicator, 8>, 8>& pieces_indicator,
	const sf::Vector2i& pos) noexcept {

	// check if field is valid and can be captured
	bool is_valid = Board::isValidField(pos) and
		(pieces_indicator[pos.y][pos.x].type == PieceFlags::PieceType::EMPTY or
			pieces_indicator[pos.y][pos.x].color != piece_color);

	if (!is_valid) {
		return false;
	}
	
	// check if field is not occupied by enemy pawn
	const auto& new_field(pieces_indicator[pos.y][pos.x]);

	if (piece_color == PieceFlags::PieceColor::WHITE) {
		return !new_field.occuping_color.black;
	}
	return !new_field.occuping_color.white;
}