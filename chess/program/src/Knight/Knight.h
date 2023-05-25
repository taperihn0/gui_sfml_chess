#pragma once

#include "..\Pieces\Piece.h"

#include <array>

class Knight final : public Piece {
public:
	Knight(const std::string& texture_path, Board* board_ptr,
		const uint16_t& size, const bool& is_white_flag);

	std::vector<sf::Vector2i>&& GetActiveFields(
		const std::array<std::array<PieceFlags::Indicator, 8>, 8>& pieces_indicator,
		const sf::Vector2i& pos) override;
private:
	void AvaibleMovesCaptures(const std::array<std::array<PieceFlags::Indicator, 8>, 8>& pieces_indicator);

	bool CheckMoveCaptureField(const std::array<std::array<PieceFlags::Indicator, 8>, 8>& pieces_indicator,
		const sf::Vector2i& pos) noexcept;

	struct delta_directions {
		uint8_t d_x;
		uint8_t d_y;
	} directions;
};

