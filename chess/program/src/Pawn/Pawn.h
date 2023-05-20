#pragma once

#include "..\Pieces\Piece.h"

class Pawn final : public Piece {
public:
	Pawn(const sf::Vector2i& pos, const std::string& texture_path, Board* board_ptr,
		const uint16_t& size, const bool &is_white_flag);

	//void ShowPossibleFields() override;
private:
	std::vector<sf::Vector2i> move_fields;
	
	bool is_white;
	const int8_t& direction;
};

