#pragma once

#include "..\Pieces\Piece.h"

#include <array>

class King final : public Piece {
public:
	King(const std::string& texture_path, Board* board_ptr,
		const uint16_t& size, bool is_white_flag);

	// return active fields of king - all of the surrounding fields, except those 
	// controlled by enemy
	const std::vector<sf::Vector2i>& GetActiveFields(
		const PieceFlags::board_grid_t& pieces_indicator,
		const sf::Vector2i& pos, bool consider_check = true, bool clear = true) override;

	void MarkOccupiedFields(
		PieceFlags::board_grid_t& board,
		const sf::Vector2i& pos, bool consider_check = true) override;
private:
	// check whether new field is safe for king
	bool CheckFieldCheckSafeValid(
		const PieceFlags::board_grid_t& pieces_indicator,
		sf::Vector2i old_pos, sf::Vector2i new_pos);

	// check castling scenario
	// conditions: 
	// * king can't be attacked
	// * it's has to be first move of rook and king
	// * between these pieces has to be free space
	void CheckAppendCastleMove(
		const PieceFlags::board_grid_t& pieces_indicator,
		sf::Vector2i pos);

	bool CheckFieldOccuped(PieceFlags::Indicator field);
	
	struct dim_directions {
		int8_t d_x, d_y;
	};

	std::array<dim_directions, 8> directions;
};