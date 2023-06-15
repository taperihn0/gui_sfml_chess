#include "Pawn.h"
#include "..\Board\Board.h"

Pawn::Pawn(const std::string& texture_path, Board* board_ptr,
	const uint16_t& size, bool is_white_flag)
	: Piece(texture_path, board_ptr, size, PieceFlags::PieceColor(2 - is_white_flag)),
	direction(is_white_flag? -1 : 1)
{}


const std::vector<sf::Vector2i>& Pawn::GetActiveFields(
	const PieceFlags::board_grid_t& pieces_indicator,
	const sf::Vector2i& pos, bool consider_check, bool clear) {
	if (clear) {
		avaible_fields.clear();
	}

	sf::Vector2i temp_vec;

	is_check = consider_check;

	// append avaible moves of a piece without capturing
	temp_vec.x = pos.x, temp_vec.y = pos.y + direction;
	AvaibleMoves(pieces_indicator, pieces_indicator[pos.y][pos.x].CheckMove(0), temp_vec, pos);

	// capturing both diagonals
	temp_vec.x = pos.x - 1, temp_vec.y = pos.y + direction;
	AvaibleCaptures(pieces_indicator, temp_vec, pos);

	// capturing using en passant
	CheckAppendEnPassantCapture(pieces_indicator, pos);

	return avaible_fields;
}


bool Pawn::CheckForUpgrade(
	const PieceFlags::board_grid_t& pieces_indicator,
	const sf::Vector2i& pos) {
	return (piece_color == PieceFlags::PieceColor::WHITE and pos.y == 0) or
		(piece_color == PieceFlags::PieceColor::BLACK and pos.y == BOARD_SIZE - 1);
}


void Pawn::MarkOccupiedFields(
	PieceFlags::board_grid_t& board,
	const sf::Vector2i& pos, bool consider_check) {
	is_check = consider_check;
	sf::Vector2i temp_vec;

	// marking both diagonals of the pawn
	temp_vec.x = pos.x - 1, temp_vec.y = pos.y + direction;
	if (Board::isValidField(temp_vec)) {
		MarkSingleOccupied(board[temp_vec.y][temp_vec.x]);
	}

	temp_vec.x += 2;
	if (Board::isValidField(temp_vec)) {
		MarkSingleOccupied(board[temp_vec.y][temp_vec.x]);
	}
}


const int8_t& Pawn::GetDirection() noexcept {
	return direction;
}


void Pawn::AvaibleMoves(
	const PieceFlags::board_grid_t& pieces_indicator,
	const bool& first_move_flag, sf::Vector2i& temp_vec, sf::Vector2i pos) noexcept {

	if (!CheckFieldFreeValid(pieces_indicator, temp_vec)) {
		return;
	}

	if (!is_check or CheckCheckSafe(pieces_indicator, pos, temp_vec)) {
		avaible_fields.push_back(temp_vec);
	}

	temp_vec.y += direction;
	if (first_move_flag and CheckFieldFreeValid(pieces_indicator, temp_vec) and
		(!is_check or CheckCheckSafe(pieces_indicator, pos, temp_vec))) {
		avaible_fields.push_back(temp_vec);
	}
}


void Pawn::AvaibleCaptures(
	const PieceFlags::board_grid_t& pieces_indicator,
	sf::Vector2i& temp_vec, sf::Vector2i pos) noexcept {

	if (CheckCaptureField(pieces_indicator, temp_vec) and 
		(!is_check or CheckCheckSafe(pieces_indicator, pos, temp_vec))) {
		avaible_fields.push_back(temp_vec);
	}

	temp_vec.x += 2;
	if (CheckCaptureField(pieces_indicator, temp_vec) and
		(!is_check or CheckCheckSafe(pieces_indicator, pos, temp_vec))) {
		avaible_fields.push_back(temp_vec);
	}
}


void Pawn::CheckAppendEnPassantCapture(
	const PieceFlags::board_grid_t& pieces_indicator,
	const sf::Vector2i& pos) {

	// condition of en passant capture
	const bool green_flag = 
		(piece_color == PieceFlags::PieceColor::WHITE and pos.y == 3) or
		(piece_color == PieceFlags::PieceColor::BLACK and pos.y == 4);

	sf::Vector2i temp_vec;

	// first side
	temp_vec.x = pos.x - 1, temp_vec.y = pos.y;
	if (green_flag and CheckEnPassantPawn(pieces_indicator, temp_vec) and
		(!is_check or CheckCheckSafe(pieces_indicator, pos, temp_vec))) {
		avaible_fields.push_back(temp_vec + sf::Vector2i(0, direction));
	}

	// second side
	temp_vec.x += 2;
	if (green_flag and CheckEnPassantPawn(pieces_indicator, temp_vec) and
		(!is_check or CheckCheckSafe(pieces_indicator, pos, temp_vec))) {
		avaible_fields.push_back(temp_vec + sf::Vector2i(0, direction));
	}
}


bool Pawn::CheckCaptureField(
	const PieceFlags::board_grid_t& pieces_indicator, 
	const sf::Vector2i& pos) noexcept {
	return CheckFieldOccupied(pieces_indicator, pos, piece_color);
}


bool Pawn::CheckEnPassantPawn(
	const PieceFlags::board_grid_t& pieces_indicator,
	const sf::Vector2i& pos) noexcept {
	return Board::isValidField(pos) and
		pieces_indicator[pos.y][pos.x].color != piece_color and
		pieces_indicator[pos.y][pos.x].type == PieceFlags::PieceType::PAWN and
		pieces_indicator[pos.y][pos.x].CheckMove(1) and
		brdclass_ptr->GetEnPassantPos() == pos;
}