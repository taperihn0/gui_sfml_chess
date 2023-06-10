#pragma once

#include "..\Pieces\Piece.h"

class Knight final : public Piece {
public:
	Knight(const std::string& texture_path, Board* board_ptr,
		const uint16_t& size, bool is_white_flag);

	// return current active fields of knight piece
	std::vector<sf::Vector2i>&& GetActiveFields(
		const PieceFlags::board_grid_t& pieces_indicator,
		const sf::Vector2i& pos, bool consider_check = true, bool clear = true) override;
private:
	// check if pos + direction coordinates field is valid and safe
	void AvaibleMovesCaptures(
		const PieceFlags::board_grid_t& pieces_indicator,
		sf::Vector2i pos) noexcept;

	// check if knight can stand up straight in the field of coordinates pos
	bool CheckMoveCaptureField(
		const PieceFlags::board_grid_t& pieces_indicator,
		const sf::Vector2i& pos) noexcept;

	struct dim_directions {
		int8_t d_x, d_y;
	} directions;
};

