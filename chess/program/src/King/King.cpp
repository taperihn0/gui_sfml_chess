#include "King.h"

King::King(const std::string& texture_path, Board* board_ptr,
	const uint16_t& size)
	: Piece(texture_path, board_ptr, size)
{}