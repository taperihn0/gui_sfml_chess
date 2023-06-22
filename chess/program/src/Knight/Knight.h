#pragma once

#include "..\Pieces\Piece.h"

class Knight final : public Piece {
public:
	Knight(const std::string& texture_path, Board* board_ptr,
		uint16_t size, bool is_white_flag);

	// return current active fields of knight piece
	const std::vector<sf::Vector2i>& 
	GetActiveFields(
		const pt::board_grid_t& gboard,
		sf::Vector2i pos, sf::Vector2i, sf::Vector2i king_pos,
		const pt::PieceList& gpiece_list,
		bool consider_check = true, bool clear = true) override;
private:

	// check if pos + direction coordinates field is valid and safe
	void AvaibleMovesCaptures(
		const pt::board_grid_t& gboard,
		const pt::PieceList& gpiece_list,
		sf::Vector2i pos) noexcept;

	// check if knight can stand up straight in the field of coordinates pos
	inline bool CheckMoveCaptureField(
		const pt::board_grid_t& gboard,
		sf::Vector2i pos) noexcept;

	struct dim_directions {
		int8_t d_x, d_y;
	} directions;
};

