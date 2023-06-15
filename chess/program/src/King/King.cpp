#include "King.h"
#include "..\Board\Board.h"

// manual definition of base class member template due to linking errors
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


King::King(const std::string& texture_path, Board* board_ptr,
	const uint16_t& size, bool is_white_flag)
	: Piece(texture_path, board_ptr, size, PieceFlags::PieceColor(2 - is_white_flag)),
	directions{ { {-1, -1}, {0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0} } }
{}


const std::vector<sf::Vector2i>& 
King::GetActiveFields(
	const PieceFlags::board_grid_t& pieces_indicator,
	const sf::Vector2i& pos, bool consider_check, bool clear) {
	if (clear) {
		avaible_fields.clear();
	}

	is_check = consider_check;
	sf::Vector2i new_pos;

	for (const auto& direct : directions) {
		new_pos = pos + sf::Vector2i(direct.d_x, direct.d_y);
	
		if (CheckFieldCheckSafeValid(pieces_indicator, pos, new_pos)) {
			avaible_fields.push_back(new_pos);
		}
	}

	CheckAppendCastleMove(pieces_indicator, pos);
	return avaible_fields;
}


void King::MarkOccupiedFields(
	PieceFlags::board_grid_t& board,
	const sf::Vector2i& pos, bool consider_check) {
	is_check = consider_check;
	sf::Vector2i new_pos;

	for (const auto& direct : directions) {
		new_pos = pos + sf::Vector2i(direct.d_x, direct.d_y);

		if (CheckFieldCheckSafeValid(board, pos, new_pos)) {
			MarkSingleOccupied(board[new_pos.y][new_pos.x]);
		}
	}
}


bool King::CheckFieldCheckSafeValid(
	const PieceFlags::board_grid_t& pieces_indicator,
	sf::Vector2i old_pos, sf::Vector2i new_pos) {

	// check if field is valid and can be captured
	bool is_valid = 
		Board::isValidField(new_pos) and
		(pieces_indicator[new_pos.y][new_pos.x].type == PieceFlags::PieceType::EMPTY or
			pieces_indicator[new_pos.y][new_pos.x].color != piece_color);

	if (!is_valid) {
		return false;
	} else if (!is_check) {
		return true;
	}

	// prepare copy of current board and then generate all the occupied fields by enemy pieces
	PieceFlags::board_grid_t pieces_indicator_cpy = pieces_indicator;

	brdclass_ptr->ZeroEntireBoardOccuperColor(pieces_indicator_cpy);
	brdclass_ptr->ChangePiecePos(pieces_indicator_cpy, old_pos, new_pos);

	return LoopGenerateOccupied(std::move(pieces_indicator_cpy), [this, new_pos](const PieceFlags::board_grid_t& board) {
		if (piece_color == PieceFlags::PieceColor::WHITE and board[new_pos.y][new_pos.x].occuping_color.black) {
			return true;
		}
		else if (piece_color == PieceFlags::PieceColor::BLACK and board[new_pos.y][new_pos.x].occuping_color.white) {
			return true;
		}
		return false;
	});
}


void King::CheckAppendCastleMove(
	const PieceFlags::board_grid_t& pieces_indicator,
	sf::Vector2i pos) {
	if (brdclass_ptr->CheckKingAttacked(pieces_indicator, piece_color) or
		!pieces_indicator[pos.y][pos.x].CheckMove(0)) {
		return;
	}

	// check one side
	auto rook_field = pieces_indicator[pos.y][0];
	bool castle_flag = true;

	if (rook_field.type == PieceFlags::PieceType::ROOK and
		rook_field.color == piece_color and rook_field.CheckMove(0)) {

		for (uint8_t x = pos.x - 1; x > 0; x--) {
			if (pieces_indicator[pos.y][x].type != PieceFlags::PieceType::EMPTY or
				CheckFieldOccuped(pieces_indicator[pos.y][x])) {
				castle_flag = false;
				break;
			}
		}

		if (castle_flag) {
			avaible_fields.push_back(sf::Vector2i(0, pos.y));
		}
	}

	// check another side
	rook_field = pieces_indicator[pos.y][BOARD_SIZE - 1];

	if (rook_field.type == PieceFlags::PieceType::ROOK and
		rook_field.color == piece_color and rook_field.CheckMove(0)) {

		for (uint8_t x = pos.x + 1; x < BOARD_SIZE - 1; x++) {
			if (pieces_indicator[pos.y][x].type != PieceFlags::PieceType::EMPTY or
				CheckFieldOccuped(pieces_indicator[pos.y][x])) {
				return;
			}
		}

		avaible_fields.push_back(sf::Vector2i(BOARD_SIZE - 1, pos.y));
	}
}


bool King::CheckFieldOccuped(PieceFlags::Indicator field) {
	return piece_color == PieceFlags::PieceColor::WHITE ?
		field.occuping_color.black : field.occuping_color.white;
}