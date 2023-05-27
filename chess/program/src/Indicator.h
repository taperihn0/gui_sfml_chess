#pragma once

namespace PieceFlags {
	enum class PieceType {
		EMPTY = 0, PAWN, BISHOP, KNIGHT, ROOK, QUEEN, KING
	};

	enum class PieceColor {
		EMPTY = 0, WHITE, BLACK
	};

	struct Indicator {
		PieceColor color;
		PieceType type;
		bool first_move;

		// PieceColor active_of_color;
	};
}