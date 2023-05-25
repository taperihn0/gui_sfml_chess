#include "Rook.h"

Rook::Rook(const std::string& texture_path, Board* board_ptr,
	const uint16_t& size, const bool& is_white_flag)
	: Piece(texture_path, board_ptr, size, PieceFlags::PieceColor(is_white_flag + 1))
{}


std::vector<sf::Vector2i>&& Rook::GetActiveFields(
	const std::array<std::array<PieceFlags::Indicator, 8>, 8>& pieces_indicator,
	const sf::Vector2i& pos) {
	return std::move(std::vector<sf::Vector2i>());
}