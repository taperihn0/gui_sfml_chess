#pragma once

#include "..\Pieces\Piece.h"

class Rook final : public Piece {
public:
	Rook(const std::string& texture_path, Board* board_ptr,
		const uint16_t& size);
};

