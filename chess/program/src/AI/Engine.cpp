#include "Engine.h"
#include "..\Board\Board.h"

#ifdef PR_PERFT
#define MOVE_LOG(pos_change, m_res) MoveLog(pos_change, m_res)
#else
#define MOVE_LOG()
#endif

inline void MoveLog(AI::piece_pos_change pchng, PERFT_RES m_res) {
	std::cout << static_cast<char>('a' + pchng.old_pos.x) << BOARD_SIZE - pchng.old_pos.y 
		<< static_cast<char>('a' + pchng.new_pos.x) << BOARD_SIZE - pchng.new_pos.y << ' ' << m_res << '\n';
}


AI::Engine::Engine(
	pt::board_grid_t& board_ref, pt::PieceList& plist_ref, Board* brd_ptr, pt::templates_t* p_templates)
	: rboard(board_ref),
	rpiece_list(plist_ref),
	brdclass_ptr(brd_ptr),
	pieces_templates(p_templates),
	weights{ 0, 1, 3, 3, 5, 90 },
	move_hlp(brd_ptr, p_templates),
	is_queen_promote(false) {
	InitSquareTables();
}


PERFT_RES 
AI::Engine::GenerateBestMove(
	const pt::av_moves_board_t& m_board, uint8_t depth, bool is_white_turn,
	const sf::Vector2i en_passant_pos, const sf::Vector2i white_king_pos, const sf::Vector2i black_king_pos) {
	
	const auto start = std::chrono::high_resolution_clock::now();

	bool turn_col(1 - is_white_turn);
	std::vector<piece_pos_change> legal_moves;

	move_hlp.SetActualEnPassant(en_passant_pos);
	move_hlp.SetKingsPos(white_king_pos, black_king_pos);
	
	sf::Vector2i old_pos_tmp;
	for (uint8_t index = 0; index < m_board.size(); index++) {
		if (pt::CheckUnValidity(rpiece_list[index]) or !pt::CheckPDataColor(rpiece_list[index], turn_col)) {
			continue;
		}

		for (const auto& move : m_board[index]) {
			old_pos_tmp = sf::Vector2i(
				pt::GetXPos(rpiece_list[index]),
				pt::GetYPos(rpiece_list[index]) );
			legal_moves.emplace_back(old_pos_tmp, move);
		}
	}

	pt::board_grid_t board_cpy(rboard);
	pt::PieceList plist_cpy(rpiece_list);
	PERFT_RES res = 0;

	for (const auto& pos_change : legal_moves) {
		move_hlp.MovePiece(board_cpy, plist_cpy, pos_change.old_pos, pos_change.new_pos);

		const auto m_res = SearchEvalMove(board_cpy, plist_cpy, depth - 1, !is_white_turn);

		MOVE_LOG(pos_change, m_res);

		res += m_res;
		move_hlp.UnMovePiece(board_cpy, plist_cpy, pos_change.new_pos);
	}

	const std::chrono::duration<float> stop = std::chrono::high_resolution_clock::now() - start;
	std::cout << "Time: " << stop.count() * 1000.f << "ms" << std::endl
		<< "Depth " << static_cast<int>(depth) << ": " << res << " nodes" << std::endl;

	return res;
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


PERFT_RES 
AI::Engine::SearchEvalMove(
	pt::board_grid_t& board, pt::PieceList& plist, uint8_t depth, bool is_white_turn) {
	if (!depth) {
		return 1;
	}
	
	std::vector<piece_pos_change> legal_moves;
	GenerateLegalMoves(legal_moves, board, plist, is_white_turn);

	PERFT_RES res = 0;

	for (const auto& pos_change : legal_moves) {
		move_hlp.MovePiece(board, plist, pos_change.old_pos, pos_change.new_pos);

		res += SearchEvalMove(board, plist, depth - 1, !is_white_turn);

		move_hlp.UnMovePiece(board, plist, pos_change.new_pos);
	}

	
	return res;
}


int16_t AI::Engine::Eval(pt::board_grid_t& board, const pt::PieceList& plist) {
	int16_t eval(0);

	/*pt::PieceType type_tmp;
	for (const auto& pdata : plist) {
		if (pt::CheckUnValidity(pdata)) {
			continue;
		}
		else if (pt::CheckColor(pdata, pt::BIT_WHITE)) { // wrong function
			type_tmp = pt::GetType(board[pt::GetYPos(pdata)][pt::GetXPos(pdata)]);
			eval += 20 * weights[static_cast<uint8_t>(type_tmp)];
			eval += square_table[static_cast<uint8_t>(board[i][j].type)][i][j];
		}
		else {
			type_tmp = pt::GetType(board[pt::GetYPos(pdata)][pt::GetXPos(pdata)]);
			eval -= 20 * weights[static_cast<uint8_t>(type_tmp)];
			eval -= square_table[static_cast<uint8_t>(board[i][j].type)][i][j];
		}
	}*/

	return eval;
}


void AI::Engine::GenerateLegalMoves(
	std::vector<piece_pos_change>& legal_moves, pt::board_grid_t& board, const pt::PieceList& plist, bool is_white_turn) {
	const bool turn_col(1 - is_white_turn);
	pt::field_t field;
	sf::Vector2i pos;

	for (const auto& pdata : plist) {
		if (pt::CheckUnValidity(pdata) or !pt::CheckPDataColor(pdata, turn_col)) {
			continue;
		}
		
		pos = sf::Vector2i(pt::GetXPos(pdata), pt::GetYPos(pdata));
		field = board[pos.y][pos.x];

		auto&& l_moves = 
			(*pieces_templates)[static_cast<uint8_t>(pt::GetColor(field))][static_cast<uint8_t>(pt::GetType(field))]->
			GetActiveFields(board, pos, move_hlp.GetPrevEnPassant(), move_hlp.GetKingPos(pt::GetColor(field)), plist);

		for (const auto& move : l_moves) {
			legal_moves.emplace_back(pos, move);
		}
	}
}
