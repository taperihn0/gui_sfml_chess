#include "MoveTraits.h"
#include "..\Board\Board.h"
#include "..\Pawn\Pawn.h"

AI::MoveTraits::MoveTraits(Board* brd_ptr, sf::Vector2i& en_passant_ref, PieceFlags::templates_t* p_templates)
	: cap_piece(),
	castle_pos(),
	capture(false),
	en_passant(false),
	castle(false),
	brdclass_ptr(brd_ptr),
	board_en_passant(en_passant_ref),
	pieces_templates(p_templates)
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
		castle_pos = new_pos;
		brdclass_ptr->CastleKingChange(board, old_pos, new_pos);
	}
	else if (board[old_pos.y][old_pos.x].type == PieceFlags::PieceType::PAWN) {
		en_passant = true;

		brdclass_ptr->ChangePiecePos(board, old_pos, new_pos);
		const auto moved_piece(board[new_pos.y][new_pos.x]);

		const auto d(
			dynamic_cast<Pawn*>((*pieces_templates)[static_cast<int>(moved_piece.color)][static_cast<int>(moved_piece.type)].get())->
			GetDirection());

		auto new_en_passant_pos(brdclass_ptr->EnPassantCase(board, new_pos, moved_piece, d));
		board_en_passant = new_en_passant_pos;
	}
	else {
		brdclass_ptr->ChangePiecePos(board, old_pos, new_pos);
	}

	brdclass_ptr->SetPieceOccupiedFields(board, new_pos.y, new_pos.x, true);
}


void AI::MoveTraits::UnMovePiece(
	PieceFlags::board_grid_t& board, const sf::Vector2i old_pos, const sf::Vector2i new_pos) {
	if (capture) {
		board[old_pos.y][old_pos.x] = board[new_pos.y][new_pos.x];
		board[new_pos.y][new_pos.x] = cap_piece;
	}
	else if (castle) {
		//board[new_pos.y][new_pos.x] = 
	}
}


void AI::MoveTraits::EnPassantCase(PieceFlags::board_grid_t& board, const sf::Vector2i old_pos, const sf::Vector2i new_pos) {
	auto& moved_piece(board[old_pos.y][old_pos.x]);

	const auto d(
		dynamic_cast<Pawn*>((*pieces_templates)[static_cast<int>(moved_piece.color)][static_cast<int>(moved_piece.type)].get())->
		GetDirection());

	auto& captured(board[new_pos.y - d][new_pos.x]);
	if (new_pos.y - d == board_en_passant.y) {
		captured =
			PieceFlags::Indicator{ PieceFlags::PieceColor::EMPTY, PieceFlags::PieceType::EMPTY };
	}
}


void AI::MoveTraits::Reset() {
	cap_piece =
		PieceFlags::Indicator();
	castle_pos =
		sf::Vector2i();
	capture = false, en_passant = false, castle = false;
}