#include "King.h"
#include "..\Board\Board.h"

// manual definition of base class member template due to linking errors
template<typename T>
bool Piece::LoopGenerateOccupied(pt::board_grid_t&& gboard, const pt::PieceList& gpiece_list, T&& break_condtn) {
	for (const auto& piece : gpiece_list) {
		if (pt::CheckPieceColor(piece, piece_color) or pt::CheckUnValidity(piece)) {
			continue;
		}

		brdclass_ptr->
			SetPieceOccupiedFields(gboard, gpiece_list, pt::GetYPos(piece), pt::GetXPos(piece));

		if (break_condtn(gboard)) {
			return false;
		}
	}

	return true;
}


King::King(const std::string& texture_path, Board* board_ptr,
	uint16_t size, bool is_white_flag)
	: Piece(texture_path, board_ptr, size, static_cast<pt::PieceColor>(1 - is_white_flag)),
	directions{ { {-1, -1}, {0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0} } }
{}


const std::vector<sf::Vector2i>&
King::GetActiveFields(
	const pt::board_grid_t& gboard,
	sf::Vector2i pos, sf::Vector2i, sf::Vector2i,
	const pt::PieceList& gpiece_list,
	bool consider_check, bool clear) {

	if (clear) {
		avaible_fields.clear();
	}

	sf::Vector2i new_pos;
	is_check = consider_check;

	for (const auto& direct : directions) {
		new_pos = pos + sf::Vector2i(direct.d_x, direct.d_y);
	
		if (CheckFieldCheckSafeValid(gboard, gpiece_list, pos, new_pos)) {
			avaible_fields.push_back(new_pos);
		}
	}

	CheckAppendCastleMove(gboard, pos);
	return avaible_fields;
}


void King::MarkOccupiedFields(
	pt::board_grid_t& gboard,
	sf::Vector2i, sf::Vector2i,
	const pt::PieceList& gpiece_list,
	sf::Vector2i pos) {

	sf::Vector2i new_pos;
	is_check = false;

	for (const auto& direct : directions) {
		new_pos = pos + sf::Vector2i(direct.d_x, direct.d_y);

		if (CheckFieldCheckSafeValid(gboard, gpiece_list, pos, new_pos)) {
			MarkSingleOccupied(gboard[new_pos.y][new_pos.x]);
		}
	}
}


bool King::CheckFieldCheckSafeValid(
	const pt::board_grid_t& gboard,
	const pt::PieceList& gpiece_list,
	sf::Vector2i old_pos, sf::Vector2i new_pos) {

	// check if field is valid and can be captured
	const bool is_valid = 
		Board::isValidField(new_pos) and
		(pt::CheckType(gboard[new_pos.y][new_pos.x], pt::BIT_EMPTY) or
			!pt::CheckColor(gboard[new_pos.y][new_pos.x], pt::CastToBitColor(piece_color)));

	if (!is_valid) {
		return false;
	} else if (!is_check) {
		return true;
	}

	// prepare copy of current board and then generate all the occupied fields by enemy pieces
	pt::board_grid_t gboard_cpy = gboard;

	brdclass_ptr->ZeroEntireBoardOccuperColor(gboard_cpy);
	brdclass_ptr->ChangePiecePos(gboard_cpy, old_pos, new_pos);

	return LoopGenerateOccupied(std::move(gboard_cpy), gpiece_list, 
		[this, new_pos](const pt::board_grid_t& gboard) {
		return brdclass_ptr->CheckKingAttacked(gboard, piece_color, new_pos);
	});
}


void King::CheckAppendCastleMove(
	const pt::board_grid_t& gboard,
	sf::Vector2i pos) {

	if (brdclass_ptr->CheckKingAttacked(gboard, piece_color, pos) or pt::CheckMoveTag(gboard[pos.y][pos.x])) {
		return;
	}

	// check one side
	auto rook_field = gboard[pos.y][0];
	bool castle_flag(true);

	// condition checker
	auto con_check = [piece_col = this->piece_color](auto rook_field) {
		return pt::CheckType(rook_field, pt::BIT_ROOK) and 
			pt::CheckColor(rook_field, pt::CastToBitColor(piece_col)) and !pt::CheckMoveTag(rook_field);
	};

	if (con_check(rook_field)) {
		for (uint8_t x = pos.x - 1; x > 0; x--) {
			if (!pt::CheckType(gboard[pos.y][x], pt::BIT_EMPTY) or
				CheckFieldOccuped(gboard[pos.y][x])) {
				castle_flag = false;
				break;
			}
		}

		if (castle_flag) {
			avaible_fields.push_back(sf::Vector2i(0, pos.y));
		}
	}

	// check another side
	rook_field = gboard[pos.y][BOARD_SIZE - 1];

	if (con_check(rook_field)) {
		for (uint8_t x = pos.x + 1; x < BOARD_SIZE - 1; x++) {
			if (!pt::CheckType(gboard[pos.y][x], pt::BIT_EMPTY) or
				CheckFieldOccuped(gboard[pos.y][x])) {
				return;
			}
		}

		avaible_fields.push_back(sf::Vector2i(BOARD_SIZE - 1, pos.y));
	}
}


inline bool King::CheckFieldOccuped(pt::field_t field) {
	return (piece_color == pt::PieceColor::WHITE) ?
		pt::CheckOccuper(field, pt::BIT_OCC_BLACK) : pt::CheckOccuper(field, pt::BIT_OCC_WHITE);
}