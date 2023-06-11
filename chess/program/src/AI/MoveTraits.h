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
			PieceFlags::board_grid_t& board, const sf::Vector2i old_pos, const sf::Vector2i new_pos);

		void UnMovePiece(
			PieceFlags::board_grid_t& board, const sf::Vector2i new_pos);

		void SetEnPassantPos(sf::Vector2i brd_en_passant_pos) noexcept;
	private:
		Board* const brdclass_ptr;
		PieceFlags::templates_t* pieces_templates;
		sf::Vector2i en_passant_pos;

		PieceFlags::board_grid_t old_board;
		std::stack<sf::Vector2i> prev_en_passant;
	};
}