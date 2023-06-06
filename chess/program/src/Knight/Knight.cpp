#include "Knight.h"
#include "..\Board\Board.h"

Knight::Knight(const std::string& texture_path, Board* board_ptr,
	const uint16_t& size, bool is_white_flag)
	: Piece(texture_path, board_ptr, size, PieceFlags::PieceColor(2 - is_white_flag)),
	directions{}
{}

// return current active fields of knight piece
std::vector<sf::Vector2i>&& Knight::GetActiveFields(
	const std::array<std::array<PieceFlags::Indicator, 8>, 8>& pieces_indicator,
	const sf::Vector2i& pos, bool consider_check, bool clear) {
	if (clear) {
		avaible_fields.clear();
	}

	is_check = consider_check;

	directions = { 1, 2 };

	// generate 4 possible moves twice
	for (int i = 0; i < 2; i++) {
		AvaibleMovesCaptures(pieces_indicator, pos);

		directions.d_y = -directions.d_y;
		AvaibleMovesCaptures(pieces_indicator, pos);

		directions.d_x = -directions.d_x;
		AvaibleMovesCaptures(pieces_indicator, pos);

		directions.d_y = -directions.d_y;
		AvaibleMovesCaptures(pieces_indicator, pos);
		
		std::swap(directions.d_x, directions.d_y);
	}

	return std::move(avaible_fields);
}

// check if pos + direction coordinates field is valid and safe
void Knight::AvaibleMovesCaptures(
	const std::array<std::array<PieceFlags::Indicator, 8>, 8>& pieces_indicator,
	sf::Vector2i pos) noexcept {

	sf::Vector2i temp_vec(pos.x + directions.d_x, pos.y + directions.d_y);

	if (CheckMoveCaptureField(pieces_indicator, temp_vec) and
		(!is_check or CheckCheckSafe(pieces_indicator, pos, temp_vec))) {
		avaible_fields.push_back(temp_vec);
	}
}

// check if knight can stand up straight in the field of coordinates pos
bool Knight::CheckMoveCaptureField(
	const std::array<std::array<PieceFlags::Indicator, 8>, 8>& pieces_indicator,
	const sf::Vector2i& pos) noexcept {
	return Board::isValidField(pos) and
		(pieces_indicator[pos.y][pos.x].type == PieceFlags::PieceType::EMPTY or
			pieces_indicator[pos.y][pos.x].color != piece_color);
}