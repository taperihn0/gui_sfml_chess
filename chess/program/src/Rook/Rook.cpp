#include "Rook.h"
#include "..\Board\Board.h"

Rook::Rook(const std::string& texture_path, Board* board_ptr,
	const uint16_t& size, const bool& is_white_flag)
	: Piece(texture_path, board_ptr, size, PieceFlags::PieceColor(2 - is_white_flag)),
	directions{ { {0, 1}, {0, -1}, {1, 0}, {-1, 0} } }
{}

// return avaible fields for rook - 
// some kind of straight lines
std::vector<sf::Vector2i>&& Rook::GetActiveFields(
	const std::array<std::array<PieceFlags::Indicator, 8>, 8>& pieces_indicator,
	const sf::Vector2i& pos, bool consider_mate, const bool& clear) {
	if (clear) {
		avaible_fields.clear();
	}

	is_mate = consider_mate;

	for (const auto& direct : directions) {
		ProcessLine(pieces_indicator, pos, sf::Vector2i(direct.d_x, direct.d_y));
	}

	return std::move(avaible_fields);
}

// go through straight line of rook's active fields
void Rook::ProcessLine(
	const std::array<std::array<PieceFlags::Indicator, 8>, 8>& pieces_indicator,
	sf::Vector2i pos, const sf::Vector2i& direction) noexcept {

	sf::Vector2i temp_vec(pos + direction);

	while (CheckFieldFreeValid(pieces_indicator, temp_vec) and
		(!is_mate or CheckMateSafe(pieces_indicator, pos, temp_vec))) {
		avaible_fields.push_back(temp_vec);
		temp_vec += direction;
	}

	if (CheckFieldOccupied(pieces_indicator, temp_vec, piece_color) and
		(!is_mate or CheckMateSafe(pieces_indicator, pos, temp_vec))) {
		avaible_fields.push_back(temp_vec);
	}
}