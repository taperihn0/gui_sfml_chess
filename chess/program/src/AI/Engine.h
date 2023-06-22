#pragma once

#include "../PieceData.h"
#include "MoveTraits.h"

#include <vector>
#include <chrono>

#define PERFT_RES unsigned long long

class Board;

namespace AI {
	class Engine {
	public:
		using square_table_t = std::array<std::array<int8_t, BOARD_SIZE>, BOARD_SIZE>;

		Engine(
			pt::board_grid_t& board_ref, pt::PieceList& plist_ref, Board* brd_ptr, pt::templates_t* p_templates);

		PERFT_RES
		GenerateBestMove(
			const pt::av_moves_board_t& m_board, uint8_t depth, bool is_white_turn, 
			const sf::Vector2i en_passant_pos, const sf::Vector2i white_king_pos, const sf::Vector2i black_king_pos);

		bool GetPromotePiece() noexcept;
	private:
		void InitSquareTables();

		bool CompEvals(int16_t ev1, int16_t ev2, bool is_white_turn) noexcept;

		PERFT_RES 
		SearchEvalMove(
			pt::board_grid_t& board, pt::PieceList& plist, uint8_t depth, bool is_white_turn);

		int16_t Eval(pt::board_grid_t& board, const pt::PieceList& plist);

		void GenerateLegalMoves(
			std::vector<piece_pos_change>& legal_moves, pt::board_grid_t& board, const pt::PieceList& plist, bool is_white_turn);

		const pt::board_grid_t& rboard;
		const pt::PieceList& rpiece_list;
		Board* const brdclass_ptr;

		const pt::templates_t* const pieces_templates;

		const std::array<uint8_t, 7> weights;
		std::array<square_table_t, 7> square_table;

		MoveTraits move_hlp;
		bool is_queen_promote;
	};
}

