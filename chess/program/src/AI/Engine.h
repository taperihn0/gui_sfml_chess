#pragma once

#include <SFML\Graphics.hpp>

#include "..\Indicator.h"
#include "MoveTraits.h"

#include <vector>

class Board;

namespace AI {
	class Engine {
	public:
		using square_table_t = std::array<std::array<int8_t, BOARD_SIZE>, BOARD_SIZE>;

		Engine(PieceFlags::board_grid_t& board_ref, Board* brd_ptr, PieceFlags::templates_t* p_templates);

		piece_pos_change
		GenerateBestMove(const PieceFlags::av_moves_board_t& m_board, uint16_t depth, bool is_white_turn);
	private:
		void InitSquareTables();

		bool CompEvals(int16_t ev1, int16_t ev2, bool is_white_turn) noexcept;

		int16_t SearchEvalMove(PieceFlags::board_grid_t& board, uint16_t depth, bool is_white_turn);

		int16_t Eval(PieceFlags::board_grid_t& board);

		void GenerateLegalMoves(std::vector<piece_pos_change>& legal_moves, PieceFlags::board_grid_t& board, bool is_white_turn);

		PieceFlags::board_grid_t& rboard;
		Board* const brdclass_ptr;
		sf::Vector2i curr_en_passant;

		const PieceFlags::templates_t* const pieces_templates;

		const std::array<uint8_t, 7> weights;
		std::array<square_table_t, 7> square_table;

		MoveTraits move_hlp;
	};
}

