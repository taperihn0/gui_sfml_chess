#pragma once

#include "..\Pieces\Piece.h"

class Board;

class Pawn final : public Piece {
public:
	Pawn(const std::string& texture_path, Board* board_ptr,
		const uint16_t& size, const bool &is_white_flag);

	std::vector<sf::Vector2i>&& GetActiveFields(
		const std::array<std::array<PieceFlags::Indicator, 8>, 8>& pieces_indicator,
		const sf::Vector2i& pos) override;
private:
	void AvaibleMoves(const std::array<std::array<PieceFlags::Indicator, 8>, 8>& pieces_indicator,
		const bool& first_move_flag, sf::Vector2i& temp_vec);
	void AvaibleCaptures(const std::array<std::array<PieceFlags::Indicator, 8>, 8>& pieces_indicator,
		sf::Vector2i& temp_vec);

	bool CheckCaptureField(
		const std::array<std::array<PieceFlags::Indicator, 8>, 8>& pieces_indicator,
		const sf::Vector2i& pos) noexcept;

	const int8_t direction;
};

