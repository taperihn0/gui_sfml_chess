#include "Rook.h"
#include "..\Board\Board.h"

Rook::Rook(const std::string& texture_path, Board* board_ptr,
	const uint16_t& size, bool is_white_flag)
	: Piece(texture_path, board_ptr, size, PieceFlags::PieceColor(2 - is_white_flag)),
	directions{ { {0, 1}, {0, -1}, {1, 0}, {-1, 0} } }
{}


std::vector<sf::Vector2i>&& Rook::GetActiveFields(
	const PieceFlags::board_grid_t& pieces_indicator,
	const sf::Vector2i& pos, bool consider_check, bool clear) {
	if (clear) {
		avaible_fields.clear();
	}

	is_check = consider_check;

	for (const auto& direct : directions) {
		ProcessLine(pieces_indicator, pos, sf::Vector2i(direct.d_x, direct.d_y));
	}

	return std::move(avaible_fields);
}


void Rook::ProcessLine(
	const PieceFlags::board_grid_t& pieces_indicator,
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