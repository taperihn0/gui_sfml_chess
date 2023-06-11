#include "MoveTraits.h"
#include "..\Board\Board.h"
#include "..\Pawn\Pawn.h"

AI::MoveTraits::MoveTraits(Board* brd_ptr, PieceFlags::templates_t* p_templates)
	: brdclass_ptr(brd_ptr),
	pieces_templates(p_templates),
	old_board()
{}


void AI::MoveTraits::MovePiece(
	PieceFlags::board_grid_t& board, const sf::Vector2i old_pos, const sf::Vector2i new_pos) {
	old_board = board;
	sf::Vector2i enpsnt_sngle_case(-1, -1);
	prev_en_passant.push(en_passant_pos);

	if (board[old_pos.y][old_pos.x].type == PieceFlags::PieceType::KING and
		board[new_pos.y][new_pos.x].type == PieceFlags::PieceType::ROOK and
		board[old_pos.y][old_pos.x].color == board[new_pos.y][new_pos.x].color) {
		brdclass_ptr->CastleKingChange(board, old_pos, new_pos);
	}
	// en passant case
	else if (board[old_pos.y][old_pos.x].type == PieceFlags::PieceType::PAWN) {
		brdclass_ptr->ChangePiecePos(board, old_pos, new_pos);
		
		const auto moved_piece(board[new_pos.y][new_pos.x]);

		const auto d(
			dynamic_cast<Pawn*>((*pieces_templates)[static_cast<int>(moved_piece.color)][static_cast<int>(moved_piece.type)].get())->
			GetDirection());
		
		if (new_pos.y - d == en_passant_pos.y) {
			board[new_pos.y - d][new_pos.x] =
				PieceFlags::Indicator{ PieceFlags::PieceColor::EMPTY, PieceFlags::PieceType::EMPTY };
		}

		const bool is_double_field_move(
			(moved_piece.color == PieceFlags::PieceColor::WHITE and new_pos.y == 4) or
			(moved_piece.color == PieceFlags::PieceColor::BLACK and new_pos.y == 3));

		if (is_double_field_move and moved_piece.CheckMove(1)) {
			enpsnt_sngle_case = new_pos;
		}
	}
	else {
		brdclass_ptr->ChangePiecePos(board, old_pos, new_pos);
	}
	
	en_passant_pos = enpsnt_sngle_case;

	brdclass_ptr->ZeroEntireBoardOccuperColor(board);

	for (uint8_t i = 0; i < BOARD_SIZE; i++) {
		for (uint8_t j = 0; j < BOARD_SIZE; j++) {
			if (board[i][j].type != PieceFlags::PieceType::EMPTY) {
				brdclass_ptr->SetPieceOccupiedFields(board, i, j, false);
			}
		}
	}
}


void AI::MoveTraits::UnMovePiece(
	PieceFlags::board_grid_t& board, const sf::Vector2i new_pos) {
	en_passant_pos = prev_en_passant.top();
	prev_en_passant.pop();

	board = old_board;
}


void AI::MoveTraits::SetEnPassantPos(sf::Vector2i brd_en_passant_pos) noexcept {
	en_passant_pos = brd_en_passant_pos;
}