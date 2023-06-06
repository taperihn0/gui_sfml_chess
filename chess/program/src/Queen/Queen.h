#pragma once

#include "..\Rook\Rook.h"
#include "..\Bishop\Bishop.h"

class Queen final : public Rook, public Bishop {
public:
	Queen(const std::string& texture_path, Board* board_ptr,
		const uint16_t& size, bool is_white_flag);

	// return active fields of queen - 
	// mix of rook and bishop
	std::vector<sf::Vector2i>&& GetActiveFields(
		const std::array<std::array<PieceFlags::Indicator, 8>, 8>& pieces_indicator,
		const sf::Vector2i& pos, bool consider_check = true, bool clear = true) override;
};

