#pragma once

namespace PieceFlags {
	enum class PieceType: uint8_t {
		EMPTY = 0, PAWN, BISHOP, KNIGHT, ROOK, QUEEN, KING
	};

	enum class PieceColor: uint8_t {
		EMPTY = 0, WHITE, BLACK
	};

	struct Indicator {
		// checking if piece did exactly 'count' (0 - no moves so far or 1 - one move) 
		// moves in a game
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

		struct occuper_color_flags {
			bool white,
				 black;
		} occuping_color;
	};
}