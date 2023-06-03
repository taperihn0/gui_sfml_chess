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
	const sf::Vector2i& pos, bool consider_check, const bool& clear) {
	if (clear) {
		avaible_fields.clear();
	}

	is_check = consider_check;

	for (const auto& direct : directions) {
		const auto& new_pos(pos + sf::Vector2i(direct.d_x, direct.d_y));
	
		if (CheckFieldCheckSafeValid(pieces_indicator, new_pos, pos)) {
			avaible_fields.push_back(new_pos);
		}
	}

	return std::move(avaible_fields);
}

// check whether field is safe for king
bool King::CheckFieldCheckSafeValid(
	const std::array<std::array<PieceFlags::Indicator, 8>, 8>& pieces_indicator,
	const sf::Vector2i& pos, sf::Vector2i old) noexcept {

	// check if field is valid and can be captured
	bool is_valid = Board::isValidField(pos) and
		(pieces_indicator[pos.y][pos.x].type == PieceFlags::PieceType::EMPTY or
			pieces_indicator[pos.y][pos.x].color != piece_color);

	if (!is_valid) {
		return false;
	}
	else if (!is_check) {
		return true;
	}

	std::array<std::array<PieceFlags::Indicator, 8>, 8> pieces_indicator_cpy = pieces_indicator;

	board->ZeroEntireBoardOccuperColor(pieces_indicator_cpy);

	board->ChangePiecePos(pieces_indicator_cpy, old, pos);

	for (uint8_t i = 0; i < 8; i++) {
		for (uint8_t j = 0; j < 8; j++) {
			if (pieces_indicator_cpy[i][j].type != PieceFlags::PieceType::EMPTY and
				pieces_indicator_cpy[i][j].color != piece_color) {
				board->SetPieceOccupiedFields(pieces_indicator_cpy, pieces_indicator_cpy[i][j], i, j, false);
			}
		}
	}

	const auto& new_field(pieces_indicator_cpy[pos.y][pos.x]);

	if (piece_color == PieceFlags::PieceColor::WHITE) {
		return !new_field.occuping_color.black;
	}
	return !new_field.occuping_color.white;
}