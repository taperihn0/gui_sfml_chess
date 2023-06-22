#pragma once

#include "..\Rook\Rook.h"
#include "..\Bishop\Bishop.h"

class Queen final : public Rook, public Bishop {
public:
	Queen(const std::string& texture_path, Board* board_ptr,
		uint16_t size, bool is_white_flag);

	// return active fields of queen - 
	// mix of rook and bishop
	const std::vector<sf::Vector2i>& 
	GetActiveFields(
		const pt::board_grid_t& gboard,
		sf::Vector2i pos, sf::Vector2i, sf::Vector2i king_pos,
		const pt::PieceList& gpiece_list,
		bool consider_check = true, bool clear = true) override;
};

