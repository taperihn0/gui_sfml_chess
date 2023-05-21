#include "Pawn.h"

Pawn::Pawn(const std::string& texture_path, Board* board_ptr, 
	const uint16_t& size, const bool& is_white_flag)
	: Piece(texture_path, board_ptr, size), is_white(is_white_flag),
	direction(is_white? 1 : -1)
{}