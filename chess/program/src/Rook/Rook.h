#pragma once

#include "..\Pieces\Piece.h"
#include "..\Bishop\Bishop.h"

class Rook : virtual public Piece {
public:
	Rook() = default;
	Rook(const std::string& texture_path, Board* board_ptr,
		uint16_t size, bool is_white_flag);

	// return avaible fields for rook - 
	// kind of straight lines
	const std::vector<sf::Vector2i>& 
	GetActiveFields(
		const pt::board_grid_t& gboard,
		sf::Vector2i pos, sf::Vector2i, sf::Vector2i king_pos,
		const pt::PieceList& gpiece_list,
		bool consider_check = true, bool clear = true) override;
private:

	// go through straight line of rook's active fields
	void ProcessLine(
		const pt::board_grid_t& gboard,
		const pt::PieceList& gpiece_list,
		sf::Vector2i pos, sf::Vector2i direction) noexcept;
	
	struct dim_directions {
		int8_t d_x, d_y;
	};

	std::array<dim_directions, 4> directions;
};

