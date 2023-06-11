#pragma once

#include <array>
#include <memory>

constexpr uint8_t BOARD_SIZE = 8;

class Piece;

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
		bool CheckMove(const uint16_t count) const noexcept {
			return move_count == count;
		}

		void IncrementMoveCount() noexcept {
			move_count++;
		};

		PieceColor color;
		PieceType type;
		
		// move_count has to be initialized with false
		uint8_t move_count;

		struct occuper_color_flags {
			bool white,
				 black;
		} occuping_color;
	};

	using board_grid_t = std::array<std::array<PieceFlags::Indicator, BOARD_SIZE>, BOARD_SIZE>;
	using av_moves_board_t = std::array<std::array<std::vector<sf::Vector2i>, BOARD_SIZE>, BOARD_SIZE>;
	using templates_t = std::array<std::array<std::unique_ptr<Piece>, 12 + 1>, 2 + 1>;
}