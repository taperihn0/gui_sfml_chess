#pragma once

#include "..\Pieces\Piece.h"

#include <array>

class King final : public Piece {
public:
	King(const std::string& texture_path, Board* board_ptr,
		const uint16_t& size, const bool& is_white_flag);

	std::vector<sf::Vector2i>&& GetActiveFields(
		const std::array<std::array<PieceFlags::Indicator, 8>, 8>& pieces_indicator,
		const sf::Vector2i& pos, bool consider_check = true, const bool& clear = true) override;
private:
	bool CheckFieldCheckSafeValid(
		const std::array<std::array<PieceFlags::Indicator, 8>, 8>& pieces_indicator,
		sf::Vector2i old_pos, sf::Vector2i new_pos) noexcept;

	void CheckAppendCastleMove(
		const std::array<std::array<PieceFlags::Indicator, 8>, 8>& pieces_indicator,
		sf::Vector2i pos);
	
	struct dim_directions {
		int8_t d_x, d_y;
	};

	std::array<dim_directions, 8> directions;
};