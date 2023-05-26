#pragma once

#include "..\Pieces\Piece.h"
#include "..\Bishop\Bishop.h"

#include <array>

class Rook : virtual public Piece {
public:
	Rook() = default;
	Rook(const std::string& texture_path, Board* board_ptr,
		const uint16_t& size, const bool& is_white_flag);

	std::vector<sf::Vector2i>&& GetActiveFields(
		const std::array<std::array<PieceFlags::Indicator, 8>, 8>& pieces_indicator,
		const sf::Vector2i& pos, const bool& clear = true) override;
private:
	void ProcessLine(const std::array<std::array<PieceFlags::Indicator, 8>, 8>& pieces_indicator,
		sf::Vector2i pos, const sf::Vector2i& direction) noexcept;
};

