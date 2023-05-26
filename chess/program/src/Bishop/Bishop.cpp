#include "Bishop.h"
#include "..\Board\Board.h"

Bishop::Bishop(const std::string& texture_path, Board* board_ptr,
	const uint16_t& size, const bool& is_white_flag)
	: Piece(texture_path, board_ptr, size, PieceFlags::PieceColor(2 - is_white_flag))
{}

// return avaible fields of bishop - just diagonals fields
std::vector<sf::Vector2i>&& Bishop::GetActiveFields(
	const std::array<std::array<PieceFlags::Indicator, 8>, 8>& pieces_indicator,
	const sf::Vector2i& pos, const bool& clear) {
	if (clear) {
		avaible_fields.clear();
	}

	// go through diagonals and break when another piece
	// appear or when end of board
	for (const auto& d_y : { -1, 1 }) {
		for (const auto& d_x : { -1, 1 }) {
			ProcessDiagonal(pieces_indicator, pos, sf::Vector2i(d_x, d_y));
		}
	}

	return std::move(avaible_fields);
}

// check whether field is avaible - if it's valid field
// or is not occupied by different piece
bool Bishop::CheckFieldFreeValid(const std::array<std::array<PieceFlags::Indicator, 8>, 8>& pieces_indicator,
	const sf::Vector2i& pos) noexcept {
	return Board::isValidField(pos) and 
		pieces_indicator[pos.y][pos.x].type == PieceFlags::PieceType::EMPTY;
}

// check if field is valid and if it's occupied by enemy piece
bool Bishop::CheckFieldOccupied(const std::array<std::array<PieceFlags::Indicator, 8>, 8>& pieces_indicator,
	const sf::Vector2i& pos, const PieceFlags::PieceColor& piece_color_) noexcept {
	return Board::isValidField(pos) and
		pieces_indicator[pos.y][pos.x].type != PieceFlags::PieceType::EMPTY and
		pieces_indicator[pos.y][pos.x].color != piece_color_;
}

// go through diagonal by doing some calculations on given direction vector
void Bishop::ProcessDiagonal(const std::array<std::array<PieceFlags::Indicator, 8>, 8>& pieces_indicator,
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