#include "Pawn.h"
#include "..\Board\Board.h"

Pawn::Pawn(const std::string& texture_path, Board* board_ptr,
	uint16_t size, bool is_white_flag)
	: Piece(texture_path, board_ptr, size, static_cast<pt::PieceColor>(1 - is_white_flag)),
	direction(is_white_flag? -1 : 1)
{}


const std::vector<sf::Vector2i>& 
Pawn::GetActiveFields(
	const pt::board_grid_t& gboard,
	sf::Vector2i pos, sf::Vector2i ep_pos, sf::Vector2i king_pos,
	const pt::PieceList& gpiece_list,
	bool consider_check, bool clear) {

	if (clear) {
		avaible_fields.clear();
	}

	sf::Vector2i temp_vec;
	is_check = consider_check;
	brd_ep = ep_pos;
	brd_king_pos = king_pos;

	// append avaible moves of a piece without capturing
	temp_vec.x = pos.x, temp_vec.y = pos.y + direction;
	const bool first_move_flag =
		!pt::CheckMoveTag(gboard[pos.y][pos.x]) and
		(piece_color == pt::PieceColor::WHITE) ? pos.y == 6 : pos.y == 1;
	AvaibleMoves(gboard, gpiece_list, first_move_flag, temp_vec, pos);

	// capturing both diagonals
	temp_vec.x = pos.x - 1, temp_vec.y = pos.y + direction;
	AvaibleCaptures(gboard, gpiece_list, temp_vec, pos);

	// capturing using en passant
	CheckAppendEnPassantCapture(gboard, gpiece_list, pos);

	return avaible_fields;
}


bool Pawn::CheckForUpgrade(
	const pt::board_grid_t& pieces_indicator,
	sf::Vector2i pos) {
	return (piece_color == pt::PieceColor::WHITE and pos.y == 0) or
		(piece_color == pt::PieceColor::BLACK and pos.y == BOARD_SIZE - 1);
}


void Pawn::MarkOccupiedFields(
	pt::board_grid_t& gboard,
	sf::Vector2i, sf::Vector2i, const pt::PieceList&,
	sf::Vector2i pos) {

	sf::Vector2i temp_vec;
	is_check = false;

	// marking both diagonals of the pawn
	temp_vec.x = pos.x - 1, temp_vec.y = pos.y + direction;
	if (Board::isValidField(temp_vec)) {
		MarkSingleOccupied(gboard[temp_vec.y][temp_vec.x]);
	}

	temp_vec.x += 2;
	if (Board::isValidField(temp_vec)) {
		MarkSingleOccupied(gboard[temp_vec.y][temp_vec.x]);
	}
}


int8_t Pawn::GetDirection() noexcept {
	return direction;
}


void Pawn::AvaibleMoves(
	const pt::board_grid_t& gboard,
	const pt::PieceList& gpiece_list,
	bool first_move_flag, sf::Vector2i& temp_vec, sf::Vector2i pos) noexcept {

	if (!CheckFieldFreeValid(gboard, temp_vec)) {
		return;
	}

	if (!is_check or CheckCheckSafe(gboard, gpiece_list, pos, temp_vec)) {
		avaible_fields.push_back(temp_vec);
	}

	temp_vec.y += direction;
	if (first_move_flag and CheckFieldFreeValid(gboard, temp_vec) and
		(!is_check or CheckCheckSafe(gboard, gpiece_list, pos, temp_vec))) {
		avaible_fields.push_back(temp_vec);
	}
}


void Pawn::AvaibleCaptures(
	const pt::board_grid_t& gboard,
	const pt::PieceList& gpiece_list,
	sf::Vector2i& temp_vec, sf::Vector2i pos) noexcept {

	if (CheckCaptureField(gboard, temp_vec) and 
		(!is_check or CheckCheckSafe(gboard, gpiece_list, pos, temp_vec))) {
		avaible_fields.push_back(temp_vec);
	}

	temp_vec.x += 2;
	if (CheckCaptureField(gboard, temp_vec) and
		(!is_check or CheckCheckSafe(gboard, gpiece_list, pos, temp_vec))) {
		avaible_fields.push_back(temp_vec);
	}
}


void Pawn::CheckAppendEnPassantCapture(
	const pt::board_grid_t& gboard,
	const pt::PieceList& gpiece_list,
	sf::Vector2i pos) {

	// temporary vector object
	sf::Vector2i temp_vec;

	// first side
	temp_vec.x = pos.x - 1, temp_vec.y = pos.y;
	if (CheckEnPassantPawn(gboard, temp_vec) and
		(!is_check or CheckCheckSafe(gboard, gpiece_list, pos, temp_vec))) {
		avaible_fields.push_back(temp_vec + sf::Vector2i(0, direction));
	}

	// second side
	temp_vec.x += 2;
	if (CheckEnPassantPawn(gboard, temp_vec) and
		(!is_check or CheckCheckSafe(gboard, gpiece_list, pos, temp_vec))) {
		avaible_fields.push_back(temp_vec + sf::Vector2i(0, direction));
	}
}


bool Pawn::CheckCaptureField(
	const pt::board_grid_t& gboard, 
	sf::Vector2i pos) noexcept {
	return CheckFieldOccupied(gboard, pos, piece_color);
}


bool Pawn::CheckEnPassantPawn(
	const pt::board_grid_t& gboard,
	sf::Vector2i pos) noexcept {
	return 
		Board::isValidField(pos) and
		!pt::CheckColor(gboard[pos.y][pos.x], pt::CastToBitColor(piece_color)) and
		 pt::CheckType(gboard[pos.y][pos.x], pt::BIT_PAWN) and
		!pt::CheckMoveTag(gboard[pos.y][pos.x]) and
		brd_ep == pos;
}