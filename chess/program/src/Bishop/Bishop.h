#pragma once

#include "..\Pieces\Piece.h"

class Bishop : virtual public Piece {
public:
	Bishop() = default;
	Bishop(const std::string& texture_path, Board* board_ptr,
		uint16_t size, bool is_white_flag);

	// return avaible fields of bishop - just diagonals fields
	const std::vector<sf::Vector2i>& 
	GetActiveFields(
		const pt::board_grid_t& gboard,
		sf::Vector2i pos, sf::Vector2i, sf::Vector2i king_pos,
		const pt::PieceList& gpiece_list,
		bool consider_check = true, bool clear = true) override;
private:

	// go through diagonal by doing some calculations on a given direction vector
	void ProcessDiagonal(
		const pt::board_grid_t& gboard,
		const pt::PieceList& gpiece_list,
		sf::Vector2i pos, const sf::Vector2i& direction) noexcept;
};

