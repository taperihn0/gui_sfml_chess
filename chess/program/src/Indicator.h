#pragma once

namespace PieceFlags {
	enum class PieceType {
		EMPTY = 0, PAWN, BISHOP, KNIGHT, ROOK, QUEEN, KING
	};

	enum class PieceColor {
		EMPTY = 0, WHITE, BLACK
	};

	struct Indicator {
		// checking if piece did 
		// exactly 'count' (0 or 1) moves in a game
		bool CheckMove(const uint16_t& count) const noexcept {
			return move_count == count;
		}

		void IncrementMoveCount() noexcept {
			move_count++;
		};

		PieceColor color;
		PieceType type;
		
		// move_count has to be initialized with 0
		uint16_t move_count;

		// PieceColor active_of_color;
	};
}