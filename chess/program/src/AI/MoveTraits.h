#pragma once

#include <SFML/Graphics.hpp>

#include "../Indicator.h"

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
		MoveTraits(Board* brdclass_ptr);

		void MovePiece(
			PieceFlags::board_grid_t& board, const sf::Vector2i old_pos, const sf::Vector2i new_pos);

		void UnMovePiece(
			PieceFlags::board_grid_t& board, const sf::Vector2i old_pos, const sf::Vector2i new_pos);
	private:
		void Reset();

		PieceFlags::Indicator cap_piece;
		bool capture, en_passant, castle;

		Board* const brdclass_ptr;
	};
}