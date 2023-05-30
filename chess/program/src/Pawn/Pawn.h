#pragma once

#include "..\Pieces\Piece.h"

class Board;

class Pawn final : public Piece {
public:
	Pawn(const std::string& texture_path, Board* board_ptr,
		const uint16_t& size, const bool &is_white_flag);

	std::vector<sf::Vector2i>&& GetActiveFields(
		const std::array<std::array<PieceFlags::Indicator, 8>, 8>& pieces_indicator,
		const sf::Vector2i& pos, const bool& clear = true) override;

	bool CheckForUpgrade(
		const std::array<std::array<PieceFlags::Indicator, 8>, 8>& pieces_indicator,
		const sf::Vector2i& pos);

	const short& GetDirection() noexcept;
private:
	void AvaibleMoves(
		const std::array<std::array<PieceFlags::Indicator, 8>, 8>& pieces_indicator,
		const bool& first_move_flag, sf::Vector2i& temp_vec) noexcept;

	void AvaibleCaptures(
		const std::array<std::array<PieceFlags::Indicator, 8>, 8>& pieces_indicator,
		sf::Vector2i& temp_vec) noexcept;

	void CheckAppendEnPassantCapture(
		const std::array<std::array<PieceFlags::Indicator, 8>, 8>& pieces_indicator,
		const sf::Vector2i& pos);

	bool CheckCaptureField(
		const std::array<std::array<PieceFlags::Indicator, 8>, 8>& pieces_indicator,
		const sf::Vector2i& pos) noexcept;

	bool CheckOccupiedFieldPawn(
		const std::array<std::array<PieceFlags::Indicator, 8>, 8>& pieces_indicator,
		const sf::Vector2i& pos) noexcept;

	const short direction;
	uint16_t move_count;
};

