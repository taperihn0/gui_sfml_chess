#include "Pawn.h"
#include "..\Board\Board.h"

Pawn::Pawn(const std::string& texture_path, Board* board_ptr,
	const uint16_t& size, const bool& is_white_flag)
	: Piece(texture_path, board_ptr, size, PieceFlags::PieceColor(2 - is_white_flag)),
	direction(is_white_flag? -1 : 1)
{}

// return active fields of path - its move fields in front 
// of the pawn and diagonal fields if capture is avaible
std::vector<sf::Vector2i>&& Pawn::GetActiveFields(
	const std::array<std::array<PieceFlags::Indicator, 8>, 8>& pieces_indicator,
	const sf::Vector2i& pos, bool consider_mate, const bool& clear) {
	if (clear) {
		avaible_fields.clear();
	}

	sf::Vector2i temp_vec;

	is_mate = consider_mate;

	// append avaible moves of a piece without capturing
	temp_vec.x = pos.x, temp_vec.y = pos.y + direction;
	AvaibleMoves(pieces_indicator, pieces_indicator[pos.y][pos.x].CheckMove(0), temp_vec, pos);

	// capturing both diagonals
	temp_vec.x = pos.x - 1, temp_vec.y = pos.y + direction;
	AvaibleCaptures(pieces_indicator, temp_vec, pos);

	// capturing using en passant
	CheckAppendEnPassantCapture(pieces_indicator, pos);

	return std::move(avaible_fields);
}

// checking whether pawn can be upgraded (standing on the last line)
bool Pawn::CheckForUpgrade(
	const std::array<std::array<PieceFlags::Indicator, 8>, 8>& pieces_indicator,
	const sf::Vector2i& pos) {
	return (piece_color == PieceFlags::PieceColor::WHITE and pos.y == 0) or
		(piece_color == PieceFlags::PieceColor::BLACK and pos.y == 8 - 1);
}

// mark occupied fields of pawn (both diagonals)
void Pawn::MarkOccupiedFields(
	std::array<std::array<PieceFlags::Indicator, 8>, 8>& board,
	const sf::Vector2i& pos, bool consider_mate) {

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

// return moving direction of the pawn - 
// it actually depends on the color of the pawn
const short& Pawn::GetDirection() noexcept {
	return direction;
}

// compute avaible moves and append them
void Pawn::AvaibleMoves(
	const std::array<std::array<PieceFlags::Indicator, 8>, 8>& pieces_indicator,
	const bool& first_move_flag, sf::Vector2i& temp_vec, sf::Vector2i pos) noexcept {
	if (!CheckFieldFreeValid(pieces_indicator, temp_vec)) {
		return;
	}

	if (!is_mate or CheckMateSafe(pieces_indicator, pos, temp_vec)) {
		avaible_fields.push_back(temp_vec);
	}

	temp_vec.y += direction;
	if (first_move_flag and CheckFieldFreeValid(pieces_indicator, temp_vec) and
		(!is_mate or CheckMateSafe(pieces_indicator, pos, temp_vec))) {
		avaible_fields.push_back(temp_vec);
	}
}

// append avaible captures for pawn 
// and check if en passant capture is avaible
void Pawn::AvaibleCaptures(
	const std::array<std::array<PieceFlags::Indicator, 8>, 8>& pieces_indicator,
	sf::Vector2i& temp_vec, sf::Vector2i pos) noexcept {
	if (CheckCaptureField(pieces_indicator, temp_vec) and 
		(!is_mate or CheckMateSafe(pieces_indicator, pos, temp_vec))) {
		avaible_fields.push_back(temp_vec);
	}

	temp_vec.x += 2;
	if (CheckCaptureField(pieces_indicator, temp_vec) and
		(!is_mate or CheckMateSafe(pieces_indicator, pos, temp_vec))) {
		avaible_fields.push_back(temp_vec);
	}
}

// check and eventually append possible en passant capture field
void Pawn::CheckAppendEnPassantCapture(
	const std::array<std::array<PieceFlags::Indicator, 8>, 8>& pieces_indicator,
	const sf::Vector2i& pos) {

	// condition of en passant capture
	const bool& green_flag = (piece_color == PieceFlags::PieceColor::WHITE and pos.y == 3) or
		(piece_color == PieceFlags::PieceColor::BLACK and pos.y == 4);

	sf::Vector2i temp_vec;

	// first side
	temp_vec.x = pos.x - 1, temp_vec.y = pos.y;
	if (green_flag and CheckEnPassantPawn(pieces_indicator, temp_vec) and
		(!is_mate or CheckMateSafe(pieces_indicator, pos, temp_vec))) {
		avaible_fields.push_back(temp_vec + sf::Vector2i(0, direction));
	}

	// second side
	temp_vec.x += 2;
	if (green_flag and CheckEnPassantPawn(pieces_indicator, temp_vec) and
		(!is_mate or CheckMateSafe(pieces_indicator, pos, temp_vec))) {
		avaible_fields.push_back(temp_vec + sf::Vector2i(0, direction));
	}
}

// just check if the field can be captured by the pawn
bool Pawn::CheckCaptureField(
	const std::array<std::array<PieceFlags::Indicator, 8>, 8>& pieces_indicator, 
	const sf::Vector2i& pos) noexcept {
	return CheckFieldOccupied(pieces_indicator, pos, piece_color);
}

// checking whether field is occupied by enemy pawn
// and whether it was his first move there
bool Pawn::CheckEnPassantPawn(
	const std::array<std::array<PieceFlags::Indicator, 8>, 8>& pieces_indicator,
	const sf::Vector2i& pos) noexcept {
	return Board::isValidField(pos) and
		pieces_indicator[pos.y][pos.x].color != piece_color and
		pieces_indicator[pos.y][pos.x].type == PieceFlags::PieceType::PAWN and
		pieces_indicator[pos.y][pos.x].CheckMove(1) and
		board->GetEnPassantPos() == pos;
}