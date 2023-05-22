#pragma once

#include "..\Pieces\Piece.h"

class King final : public Piece {
public:
	King(const std::string& texture_path, Board* board_ptr,
		const uint16_t& size);
};

