#pragma once

#include <SFML/Graphics.hpp>

#include "../Indicator.h"

#include <stack>

class Board;

namespace AI {
	struct piece_pos_change {
		piece_pos_change() = default;
		piece_pos_change(sf::Vector2i old_pos_, sf::Vector2i new_pos_)
			: old_pos(old_pos_), new_pos(new_pos_)
		{}

		sf::Vector2i old_pos,
			new_pos;
	};


	class MoveTraits {
	public:
		MoveTraits(Board* brdclass_ptr, PieceFlags::templates_t* p_templates);

		void MovePiece(
			PieceFlags::board_grid_t& board, const sf::Vector2i old_pos, sf::Vector2i new_pos);

		void UnMovePiece(
			PieceFlags::board_grid_t& board, const sf::Vector2i new_pos);

		void SetActualEnPassant(const sf::Vector2i new_en_passant) noexcept;

		void SetKingsPos(const sf::Vector2i white_king, const sf::Vector2i black_king);

		bool CheckPromote() noexcept;

		bool CheckKingAttacked(PieceFlags::board_grid_t& board, const PieceFlags::PieceColor king_col);

		sf::Vector2i GetPrevEnPassant();

		sf::Vector2i GetKingPos(PieceFlags::PieceColor king_col);
	private:
		void UpdateBoard(PieceFlags::board_grid_t& board);

		Board* const brdclass_ptr;
		PieceFlags::templates_t* pieces_templates;

		sf::Vector2i
			en_passant_pos,
			white_king_pos,
			black_king_pos;

		PieceFlags::board_grid_t old_board;
		std::stack<sf::Vector2i>
			prev_en_passant,
			prev_white_king,
			prev_black_king;

		std::stack<PieceFlags::board_grid_t> prev_brd_states;

		bool promote_flag;
	};
}