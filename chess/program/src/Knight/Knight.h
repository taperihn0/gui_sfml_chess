#pragma once

#include "..\Pieces\Piece.h"

class Knight final : public Piece {
public:
	Knight(const std::string& texture_path, Board* board_ptr,
		const uint16_t& size);
};

