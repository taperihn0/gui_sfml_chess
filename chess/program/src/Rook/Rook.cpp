#include "Rook.h"

Rook::Rook(const std::string& texture_path, Board* board_ptr,
	const uint16_t& size)
	: Piece(texture_path, board_ptr, size)
{}