#pragma once

#include "..\Pieces\Piece.h"

class Queen final : public Piece {
public:
	Queen(const std::string& texture_path, Board* board_ptr,
		const uint16_t& size);
};

