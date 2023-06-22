#pragma once

#include "..\Pieces\Piece.h"

class Pawn final : public Piece {
public:
	Pawn(const std::string& texture_path, Board* board_ptr,
		uint16_t size, bool is_white_flag);

	// return active fields of path - its move fields in front 
	// of the pawn and diagonal fields if capture is avaible
	const std::vector<sf::Vector2i>& 
	GetActiveFields(
		const pt::board_grid_t& gboard,
		sf::Vector2i pos, sf::Vector2i ep_pos, sf::Vector2i king_pos,
		const pt::PieceList& gpiece_list,
		bool consider_check = true, bool clear = true) override;

	// check whether pawn can be upgraded (standing on the last line)
	bool CheckForUpgrade(
		const pt::board_grid_t& gboard,
		sf::Vector2i pos);

	// mark occupied fields of pawn (both diagonals)
	void MarkOccupiedFields(
		pt::board_grid_t& gboard,
		sf::Vector2i, sf::Vector2i,
		const pt::PieceList&,
		sf::Vector2i pos) override;

	// return moving direction of the pawn - 
	// it actually depends on the color of the pawn
	int8_t GetDirection() noexcept;
private:

	// compute avaible moves and append them
	void AvaibleMoves(
		const pt::board_grid_t& gboard,
		const pt::PieceList& piece_list,
		bool first_move_flag, sf::Vector2i& temp_vec, sf::Vector2i pos) noexcept;

	// append avaible captures for pawn 
	// and check if en passant capture is avaible
	void AvaibleCaptures(
		const pt::board_grid_t& gboard,
		const pt::PieceList& piece_list,
		sf::Vector2i& temp_vec, sf::Vector2i pos) noexcept;

	// check and eventually append possible en passant capture field
	void CheckAppendEnPassantCapture(
		const pt::board_grid_t& gboard,
		const pt::PieceList& piece_list,
		sf::Vector2i pos);
	
	// just check if the field can be captured by the pawn
	bool CheckCaptureField(
		const pt::board_grid_t& gboard,
		sf::Vector2i pos) noexcept;

	// checking whether field is occupied by enemy pawn
	// and whether it was his first move there
	bool CheckEnPassantPawn(
		const pt::board_grid_t& gboard,
		sf::Vector2i pos) noexcept;

	const int8_t direction;
};