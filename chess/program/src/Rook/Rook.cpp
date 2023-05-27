#include "Rook.h"

Rook::Rook(const std::string& texture_path, Board* board_ptr,
	const uint16_t& size, const bool& is_white_flag)
	: Piece(texture_path, board_ptr, size, PieceFlags::PieceColor(2 - is_white_flag))
{}

// return avaible fields for rook - 
// some kind of straight lines
std::vector<sf::Vector2i>&& Rook::GetActiveFields(
	const std::array<std::array<PieceFlags::Indicator, 8>, 8>& pieces_indicator,
	const sf::Vector2i& pos, const bool& clear) {
	if (clear) {
		avaible_fields.clear();
	}

	sf::Vector2i direction;
	std::array<std::array<int, 2>, 4> direct_array = { { {0, 1}, {0, -1}, {1, 0}, {-1, 0} } };

	for (const auto& directions : direct_array) {
		direction.x = directions[0];
		direction.y = directions[1];

		ProcessLine(pieces_indicator, pos, direction);
	}

	return std::move(avaible_fields);
}

// go through straight line of rook's active fields
void Rook::ProcessLine(
	const std::array<std::array<PieceFlags::Indicator, 8>, 8>& pieces_indicator,
	sf::Vector2i pos, const sf::Vector2i& direction) noexcept {
	pos += direction;

	while (CheckFieldFreeValid(pieces_indicator, pos)) {
		avaible_fields.push_back(pos);
		pos += direction;
	}

	if (CheckFieldOccupied(pieces_indicator, pos, piece_color)) {
		avaible_fields.push_back(pos);
	}
}