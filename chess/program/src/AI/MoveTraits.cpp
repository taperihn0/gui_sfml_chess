#include "MoveTraits.h"
#include "..\Board\Board.h"
#include "..\Pawn\Pawn.h"

AI::MoveTraits::MoveTraits(Board* brd_ptr, PieceFlags::templates_t* p_templates)
	: brdclass_ptr(brd_ptr),
	pieces_templates(p_templates),
	old_board{},
	promote_flag(false)
{}


void AI::MoveTraits::MovePiece(
	PieceFlags::board_grid_t& board, const sf::Vector2i old_pos, sf::Vector2i new_pos) {

	// remembering old board and then using this as an UnMove operation
	old_board = board;
	sf::Vector2i enpsnt_sngle_case;

	prev_en_passant.push(en_passant_pos);
	prev_white_king.push(white_king_pos);
	prev_black_king.push(black_king_pos);

	promote_flag = false;
	auto
		moved_piece(board[old_pos.y][old_pos.x]),
		old_piece(board[new_pos.y][new_pos.x]);

	// checking for castling action
	if (moved_piece.type == PieceFlags::PieceType::KING and
		old_piece.type == PieceFlags::PieceType::ROOK and old_piece.color == moved_piece.color) {
		brdclass_ptr->CastleKingChange(board, old_pos, new_pos);
	}
	else {
		brdclass_ptr->ChangePiecePos(board, old_pos, new_pos);
	}

	brdclass_ptr->CheckUpdateIfKingMove(board, new_pos, white_king_pos, black_king_pos);

	moved_piece = board[new_pos.y][new_pos.x];

	// moving a heavy piece resets current en passant pos
	if (moved_piece.type != PieceFlags::PieceType::PAWN) {
		enpsnt_sngle_case = sf::Vector2i(-1, -1);
	}
	// Check if the moved piece was pawn - this case is a bit tricky
	else if (moved_piece.type == PieceFlags::PieceType::PAWN) {
		auto* pawn_t_ptr =
			dynamic_cast<Pawn*>((*pieces_templates)[static_cast<uint8_t>(moved_piece.color)][static_cast<uint8_t>(moved_piece.type)].get());

		const auto d = pawn_t_ptr->GetDirection();

		enpsnt_sngle_case = brdclass_ptr->EnPassantCase(board, new_pos, moved_piece, d);

		// check whether its new field is in the last row -
		// it means pawn can be upgraded

		UpdateBoard(board);

		if (pawn_t_ptr->CheckForUpgrade(board, new_pos)) {
			promote_flag = true;
		}

		return;
	}

	UpdateBoard(board);
	SetActualEnPassant(enpsnt_sngle_case);
}


void AI::MoveTraits::UnMovePiece(
	PieceFlags::board_grid_t& board, const sf::Vector2i new_pos) {

	// getting last traits and popping it
	en_passant_pos = prev_en_passant.top();
	prev_en_passant.pop();

	white_king_pos = prev_white_king.top();
	prev_white_king.pop();

	black_king_pos = prev_black_king.top();
	prev_black_king.pop();

	// last board state
	board = old_board;
}


void AI::MoveTraits::SetActualEnPassant(const sf::Vector2i new_en_passant) noexcept {
	en_passant_pos = new_en_passant;
}


void AI::MoveTraits::SetKingsPos(const sf::Vector2i white_king, const sf::Vector2i black_king) {
	white_king_pos = white_king;
	black_king_pos = black_king;
}


bool AI::MoveTraits::CheckPromote() noexcept {
	return promote_flag;
}


bool AI::MoveTraits::CheckKingAttacked(PieceFlags::board_grid_t& board, const PieceFlags::PieceColor king_col) {
	return king_col == PieceFlags::PieceColor::WHITE ?
		board[white_king_pos.y][white_king_pos.x].occuping_color.black :
		board[black_king_pos.y][black_king_pos.x].occuping_color.white;
}


void AI::MoveTraits::UpdateBoard(PieceFlags::board_grid_t& board) {
	for (uint8_t i = 0; i < BOARD_SIZE; i++) {
		for (uint8_t j = 0; j < BOARD_SIZE; j++) {
			if (board[i][j].type == PieceFlags::PieceType::EMPTY) {
				continue;
			}

			brdclass_ptr->SetPieceOccupiedFields(board, i, j, false);
		}
	}
}