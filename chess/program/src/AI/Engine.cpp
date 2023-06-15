#include "Engine.h"
#include "..\Board\Board.h"

AI::Engine::Engine(PieceFlags::board_grid_t& board_ref, Board* brd_ptr, PieceFlags::templates_t* p_templates)
	: rboard(board_ref),
	brdclass_ptr(brd_ptr),
	pieces_templates(p_templates),
	weights{ 0, 1, 3, 3, 5, 90 },
	move_hlp(brd_ptr, p_templates),
	is_queen_promote(false) {
	InitSquareTables();
}


AI::piece_pos_change
AI::Engine::GenerateBestMove(
	const PieceFlags::av_moves_board_t& m_board, uint8_t depth, bool is_white_turn, 
	const sf::Vector2i en_passant_pos, const sf::Vector2i white_king_pos, const sf::Vector2i black_king_pos) {

	// setting color of the current player which got the turn and resetting promote flag
	auto turn_col = static_cast<PieceFlags::PieceColor>(2 - is_white_turn);
	std::vector<piece_pos_change> legal_moves;
	is_queen_promote = false;

	move_hlp.SetActualEnPassant(en_passant_pos);
	move_hlp.SetKingsPos(white_king_pos, black_king_pos);

	for (uint8_t i = 0; i < BOARD_SIZE; i++) {
		for (uint8_t j = 0; j < BOARD_SIZE; j++) {
			if (turn_col != rboard[i][j].color) {
				continue;
			}

			for (const auto& move : m_board[i][j]) {
				legal_moves.emplace_back(sf::Vector2i{ j, i }, move);
			}
		}
	}

	int16_t best_pos_eval = is_white_turn ? INT16_MIN : INT16_MAX;
	piece_pos_change final_move;
	PieceFlags::board_grid_t board_cpy = rboard;

	for (const auto& pos_change : legal_moves) {
		move_hlp.MovePiece(board_cpy, pos_change.old_pos, pos_change.new_pos);
		
		if (move_hlp.CheckPromote()) {
			board_cpy[pos_change.new_pos.y][pos_change.new_pos.x] =
				PieceFlags::Indicator{ static_cast<PieceFlags::PieceColor>(2 - is_white_turn), PieceFlags::PieceType::QUEEN };

			const auto move_eval = SearchEvalMove(board_cpy, depth - 1, is_white_turn);

			if (CompEvals(best_pos_eval, move_eval, is_white_turn)) {
				final_move = pos_change;
				best_pos_eval = move_eval;
				is_queen_promote = true;
			}

			board_cpy[pos_change.new_pos.y][pos_change.new_pos.x] =
				PieceFlags::Indicator{ static_cast<PieceFlags::PieceColor>(2 - is_white_turn), PieceFlags::PieceType::KNIGHT };
		}

		const auto move_eval = SearchEvalMove(board_cpy, depth - 1, is_white_turn);

		if (CompEvals(best_pos_eval, move_eval, is_white_turn)) {
			final_move = pos_change;
			best_pos_eval = move_eval;
		}

		move_hlp.UnMovePiece(board_cpy, pos_change.new_pos);
	}

	// return chosen best move
	return final_move;
}


bool AI::Engine::GetPromotePiece() noexcept {
	return is_queen_promote;
}

void AI::Engine::InitSquareTables() {
	square_table[0] = {};

	square_table[1] = {{
		{  0,  0,  0,  0,  0,  0,  0,  0 },
		{ 50, 50, 50, 50, 50, 50, 50, 50 },
		{ 10, 10, 20, 30, 30, 20, 10, 10 },
		{  5,  5, 10, 25, 25, 10,  5,  5 },
		{  0,  0,  0, 20, 20,  0,  0,  0 },
		{  5, -5,-10,  0,  0,-10, -5,  5 },
		{  5, 10, 10,-20,-20, 10, 10,  5 },
		{  0,  0,  0,  0,  0,  0,  0,  0 }
	} };

	square_table[2] = {{
		{ -20,-10,-10,-10,-10,-10,-10,-20 },
		{ -10, 0, 0, 0, 0, 0, 0,-10 },
		{ -10, 0, 5, 10, 10, 5, 0,-10 },
		{ -10, 5, 5, 10, 10, 5, 5,-10 },
		{ -10, 0, 10, 10, 10, 10, 0,-10 },
		{ -10, 10, 10, 10, 10, 10, 10,-10 },
		{ -10, 5, 0, 0, 0, 0, 5,-10 },
		{ -20,-10,-10,-10,-10,-10,-10,-20 }
	} };

	square_table[3] = {{
		{ -50,-40,-30,-30,-30,-30,-40,-50 },
		{ -40,-20,  0,  0,  0,  0,-20,-40 },
		{ -30,  0, 10, 15, 15, 10,  0,-30 },
		{ -30,  5, 15, 20, 20, 15,  5,-30 },
		{ -30,  0, 15, 20, 20, 15,  0,-30 },
		{ -30,  5, 10, 15, 15, 10,  5,-30 },
		{ -40,-20,  0,  5,  5,  0,-20,-40 },
		{ -50,-40,-30,-30,-30,-30,-40,-50 }
	} };

	square_table[4] = { {
		{ 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 5, 10, 10, 10, 10, 10, 10, 5 },
		{-5, 0, 0, 0, 0, 0, 0,-5 },
		{-5, 0, 0, 0, 0, 0, 0,-5 },
		{-5, 0, 0, 0, 0, 0, 0,-5 },
		{-5, 0, 0, 0, 0, 0, 0,-5 },
		{-5, 0, 0, 0, 0, 0, 0,-5 },
		{ 0, 0, 0, 5, 5, 0, 0, 0 }
	} };

	square_table[5] = { {
		{ -20,-10,-10, -5, -5,-10,-10,-20 },
		{ -10, 0, 0, 0, 0, 0, 0,-10 },
		{ -10, 0, 5, 5, 5, 5, 0,-10 },
		{ -5, 0, 5, 5, 5, 5, 0, -5 },
		{ 0, 0, 5, 5, 5, 5, 0, -5 },
		{ -10, 5, 5, 5, 5, 5, 0,-10 },
		{ -10, 0, 5, 0, 0, 0, 0,-10 },
		{ -20,-10,-10, -5, -5,-10,-10,-20 }
	} };

	square_table[6] = { {
		{ 20, 30, 10, 0, 0, 10, 30, 20 },
		{ 20, 20, 0, 0, 0, 0, 20, 20 },
		{ -10,-20,-20,-20,-20,-20,-20,-10 },
		{ -20,-30,-30,-40,-40,-30,-30,-20 },
		{ -30,-40,-40,-50,-50,-40,-40,-30 },
		{ -30,-40,-40,-50,-50,-40,-40,-30 },
		{ -30,-40,-40,-50,-50,-40,-40,-30 },
		{ -30,-40,-40,-50,-50,-40,-40,-30 }
	} };
}


inline bool AI::Engine::CompEvals(int16_t ev1, int16_t ev2, bool is_white_turn) noexcept {
	return is_white_turn ? ev1 < ev2 : ev1 > ev2;
}


int16_t AI::Engine::SearchEvalMove(PieceFlags::board_grid_t& board, uint8_t depth, bool is_white_turn) {
	if (!depth) {
		return Eval(board);
	}
	else if (brdclass_ptr->CheckKingAttacked(board, PieceFlags::PieceColor(2 - is_white_turn))) {
		return is_white_turn ? INT16_MIN : INT16_MAX;
	}

	int16_t eval(is_white_turn ? INT16_MIN : INT16_MAX);
	std::vector<piece_pos_change> legal_moves;
	GenerateLegalMoves(legal_moves, board, is_white_turn);

	for (const auto& pos_change : legal_moves) {
		move_hlp.MovePiece(board, pos_change.old_pos, pos_change.new_pos);

		if (is_white_turn) {
			eval = std::max(eval, SearchEvalMove(board, depth - 1, !is_white_turn));
		}
		else {
			eval = std::min(eval, SearchEvalMove(board, depth - 1, !is_white_turn));
		}
	
		move_hlp.UnMovePiece(board, pos_change.new_pos);
	}

	return eval;
}


int16_t AI::Engine::Eval(PieceFlags::board_grid_t& board) {
	int16_t eval(0);

	for (uint8_t i = 0; i < BOARD_SIZE; i++) {
		for (uint8_t j = 0; j < BOARD_SIZE; j++) {
			if (board[i][j].color == PieceFlags::PieceColor::WHITE) {
				eval += 20 * weights[static_cast<uint8_t>(board[i][j].type)];
				eval += square_table[static_cast<uint8_t>(board[i][j].type)][i][j];
			}
			else {
				eval -= 20 * weights[static_cast<uint8_t>(board[i][j].type)];
				eval -= square_table[static_cast<uint8_t>(board[i][j].type)][i][j];
			}
		}
	}

	return eval;
}


void AI::Engine::GenerateLegalMoves(std::vector<piece_pos_change>& legal_moves, PieceFlags::board_grid_t& board, bool is_white_turn) {
	uint8_t turn_col(2 - is_white_turn);
	PieceFlags::Indicator piece;

	for (uint8_t i = 0; i < BOARD_SIZE; i++) {
		for (uint8_t j = 0; j < BOARD_SIZE; j++) {
			piece = board[i][j];

			if (piece.type == PieceFlags::PieceType::EMPTY or
				piece.color != static_cast<PieceFlags::PieceColor>(turn_col)) {
				continue;
			}

			auto l_moves(
				(*pieces_templates)[static_cast<uint8_t>(piece.color)][static_cast<uint8_t>(piece.type)]->
				GetActiveFields(board, sf::Vector2i(j, i)));

			for (const auto& move : l_moves) {
				legal_moves.emplace_back(sf::Vector2i{ j, i }, move);
			}
		}
	}
}
