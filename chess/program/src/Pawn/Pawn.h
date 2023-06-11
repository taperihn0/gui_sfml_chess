#pragma once

#include "..\Pieces\Piece.h"

class Pawn final : public Piece {
public:
	Pawn(const std::string& texture_path, Board* board_ptr,
		const uint16_t& size, bool is_white_flag);

	// return active fields of path - its move fields in front 
	// of the pawn and diagonal fields if capture is avaible
	const std::vector<sf::Vector2i>& GetActiveFields(
		const PieceFlags::board_grid_t& pieces_indicator,
		const sf::Vector2i& pos, bool consider_check = true, bool clear = true) override;

	// check whether pawn can be upgraded (standing on the last line)
	bool CheckForUpgrade(
		const PieceFlags::board_grid_t& pieces_indicator,
		const sf::Vector2i& pos);

	// mark occupied fields of pawn (both diagonals)
	void MarkOccupiedFields(
		PieceFlags::board_grid_t& board,
		const sf::Vector2i& pos, bool consider_check = true) override;

	// return moving direction of the pawn - 
	// it actually depends on the color of the pawn
	const int8_t& GetDirection() noexcept;
private:
	// compute avaible moves and append them
	void AvaibleMoves(
		const PieceFlags::board_grid_t& pieces_indicator,
		const bool& first_move_flag, sf::Vector2i& temp_vec, sf::Vector2i pos) noexcept;

	// append avaible captures for pawn 
	// and check if en passant capture is avaible
	void AvaibleCaptures(
		const PieceFlags::board_grid_t& pieces_indicator,
		sf::Vector2i& temp_vec, sf::Vector2i pos) noexcept;

	// check and eventually append possible en passant capture field
	void CheckAppendEnPassantCapture(
		const PieceFlags::board_grid_t& pieces_indicator,
		const sf::Vector2i& pos);
	
	// just check if the field can be captured by the pawn
	bool CheckCaptureField(
		const PieceFlags::board_grid_t& pieces_indicator,
		const sf::Vector2i& pos) noexcept;

	// checking whether field is occupied by enemy pawn
	// and whether it was his first move there
	bool CheckEnPassantPawn(
		const PieceFlags::board_grid_t& pieces_indicator,
		const sf::Vector2i& pos) noexcept;

	const int8_t direction;
};

