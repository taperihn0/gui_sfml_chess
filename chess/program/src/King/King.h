#pragma once

#include "..\Pieces\Piece.h"

class King final : public Piece {
public:
	King(const std::string& texture_path, Board* board_ptr,
		uint16_t size, bool is_white_flag);

	// return active fields of king - all of the surrounding fields, 
	// except those controlled by enemy
	const std::vector<sf::Vector2i>& 
	GetActiveFields(
		const pt::board_grid_t& gboard,
		sf::Vector2i pos, sf::Vector2i, sf::Vector2i,
		const pt::PieceList& gpiece_list,
		bool consider_check = true, bool clear = true) override;

	// like other pieces - 
	// return occupied fields by king on a given board and with passed position
	void MarkOccupiedFields(
		pt::board_grid_t& gboard,
		sf::Vector2i ep_pos, sf::Vector2i king_pos,
		const pt::PieceList& gpiece_list,
		sf::Vector2i pos) override;
private:

	// check whether new field is safe for king
	bool CheckFieldCheckSafeValid(
		const pt::board_grid_t& gboard,
		const pt::PieceList& piece_list,
		sf::Vector2i old_pos, sf::Vector2i new_pos);

	// check castling scenario
	// conditions: 
	// * king can't be attacked
	// * it's has to be first move of rook and king
	// * between these pieces has to be free space
	void CheckAppendCastleMove(
		const pt::board_grid_t& pieces_indicator,
		sf::Vector2i pos);

	inline bool CheckFieldOccuped(pt::field_t field);
	
	struct dim_directions {
		int8_t d_x, d_y;
	};

	std::array<dim_directions, 8> directions;
};