#pragma once

#include "..\Pieces\Piece.h"

class Bishop final : public Piece {
public:
	Bishop(const std::string& texture_path, Board* board_ptr,
		const uint16_t& size);
};

