#include "MoveTraits.h"
#include "..\Board\Board.h"

AI::MoveTraits::MoveTraits(Board* brd_ptr) 
	: brdclass_ptr(brd_ptr)
{}


void AI::MoveTraits::MovePiece(
	PieceFlags::board_grid_t& board, const sf::Vector2i old_pos, const sf::Vector2i new_pos) {
	Reset();

	if (board[new_pos.y][new_pos.x].type != PieceFlags::PieceType::EMPTY) {
		capture = true;
		cap_piece = board[new_pos.y][new_pos.x];
		brdclass_ptr->ChangePiecePos(board, old_pos, new_pos);

	}
	else if (board[old_pos.y][old_pos.x].type == PieceFlags::PieceType::KING and
		board[new_pos.y][new_pos.x].type == PieceFlags::PieceType::ROOK and
		board[old_pos.y][old_pos.x].color == board[new_pos.y][new_pos.x].color) {
		castle = true;
		brdclass_ptr->CastleKingChange(board, old_pos, new_pos);
	}
	else if (board[old_pos.y][old_pos.x].type == PieceFlags::PieceType::PAWN) {
		brdclass_ptr->GetEnPassantPos(board, old_pos, new_pos);
	}
}


void AI::MoveTraits::UnMovePiece(
	PieceFlags::board_grid_t& board, const sf::Vector2i old_pos, const sf::Vector2i new_pos) {

}


void AI::MoveTraits::Reset() {
	cap_piece =
		PieceFlags::Indicator{ PieceFlags::PieceColor::EMPTY, PieceFlags::PieceType::EMPTY };
	capture = false, en_passant = false, castle = false;
}