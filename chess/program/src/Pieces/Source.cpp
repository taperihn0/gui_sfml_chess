#include "Piece.h"
#include "Piece.cpp"

template<typename T>
bool Piece::LoopGenerateOccupied(PieceFlags::board_grid_t&& board, T&& break_condtn) {
	for (uint8_t i = 0; i < BOARD_SIZE; i++) {
		for (uint8_t j = 0; j < BOARD_SIZE; j++) {
			if (board[i][j].type == PieceFlags::PieceType::EMPTY or
				board[i][j].color == piece_color) {
				continue;
			}

			brdclass_ptr->SetPieceOccupiedFields(board, i, j, false);

			if (break_condtn(board)) {
				return false;
			}
		}
	}

	return true;
}