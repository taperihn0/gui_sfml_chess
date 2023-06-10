#pragma once

#include "..\Pieces\Piece.h"

class Bishop : virtual public Piece {
public:
	Bishop() = default;
	Bishop(const std::string& texture_path, Board* board_ptr,
		const uint16_t& size, bool is_white_flag);

	// return avaible fields of bishop - just diagonals fields
	std::vector<sf::Vector2i>&& GetActiveFields(
		const PieceFlags::board_grid_t& pieces_indicator,
		const sf::Vector2i& pos, bool consider_check = true, bool clear = true) override;
private:
	// go through diagonal by doing some calculations on a given direction vector
	void ProcessDiagonal(
		const PieceFlags::board_grid_t& pieces_indicator,
		sf::Vector2i pos, const sf::Vector2i& direction) noexcept;
};

