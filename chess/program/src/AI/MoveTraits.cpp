#include "MoveTraits.h"
#include "..\Board\Board.h"
#include "..\Pawn\Pawn.h"

AI::MoveTraits::MoveTraits(Board* brd_ptr, pt::templates_t* p_templates)
	: brdclass_ptr(brd_ptr),
	pieces_templates(p_templates),
	old_board{},
	promote_flag(false)
{}


void AI::MoveTraits::MovePiece(pt::board_grid_t& gboard, pt::PieceList& gpiece_list, const sf::Vector2i old_pos, sf::Vector2i new_pos) {

	// remembering old board state and then using it as an UnMove operation
	prev_en_passant.push(en_passant_pos);
	prev_white_king.push(white_king_pos);
	prev_black_king.push(black_king_pos);
	prev_brd_states.push(gboard);

	promote_flag = false;
	auto
		moved_piece(gboard[old_pos.y][old_pos.x]),
		old_piece(gboard[new_pos.y][new_pos.x]);
	pdata_change chng{ 0, false, 0 };

	// checking for castling action
	if (pt::CheckType(moved_piece, pt::BIT_KING) and
		pt::CheckType(old_piece, pt::BIT_ROOK) and pt::CheckColor(old_piece, pt::GetBitColor(moved_piece))) {
		CastleCase(gboard, gpiece_list, old_pos, new_pos, chng);
	}
	else {
		// if there is performed any capture, just delete subordinate piece from the piece list
		if (!pt::CheckType(gboard[new_pos.y][new_pos.x], pt::BIT_EMPTY)) {
			const auto cap_index = pt::GetPieceListIndex(gboard[new_pos.y][new_pos.x]);
			chng.second = true;
			chng.cpy2 = ((cap_index << 8) | gpiece_list[cap_index]);
			pt::UnSetValidity(gpiece_list[cap_index]);
		}
	
		brdclass_ptr->ChangePiecePos(gboard, old_pos, new_pos);
		const auto index = pt::GetPieceListIndex(gboard[new_pos.y][new_pos.x]);

		// remember data when list of piece is modyfied
		chng.cpy1 = ((index << 8) | gpiece_list[index]);
		gpiece_list.ChangePos(index, new_pos);
	}

	brdclass_ptr->CheckUpdateIfKingMove(gboard, new_pos, white_king_pos, black_king_pos);
	moved_piece = gboard[new_pos.y][new_pos.x];

	// Check if the moved piece was pawn - this case is a bit tricky
	if (pt::CheckType(moved_piece, pt::BIT_PAWN)) {
		auto* const pawn_t_ptr =
			dynamic_cast<Pawn*>
			((*pieces_templates)[static_cast<uint8_t>(pt::GetColor(moved_piece))][static_cast<uint8_t>(pt::GetType(moved_piece))].get());

		const auto d = pawn_t_ptr->GetDirection();

		EnPassantCase(gboard, gpiece_list, new_pos, d, chng);
		prev_plist_changes.push(chng);

		UpdateBoard(gboard, gpiece_list);

		// check whether its new field is in the last row -
		// it means pawn can be upgraded

		if (pawn_t_ptr->CheckForUpgrade(gboard, new_pos)) {
			promote_flag = true;
		}
	}
	// moving a heavy piece resets current en passant pos
	else {
		SetActualEnPassant(sf::Vector2i(-1, -1));
		prev_plist_changes.push(chng);
		UpdateBoard(gboard, gpiece_list);
	}
}


void AI::MoveTraits::UnMovePiece(pt::board_grid_t& gboard, pt::PieceList& gpiece_list, const sf::Vector2i new_pos) {

	// getting last traits and popping it
	en_passant_pos = prev_en_passant.top();
	prev_en_passant.pop();

	white_king_pos = prev_white_king.top();
	prev_white_king.pop();

	black_king_pos = prev_black_king.top();
	prev_black_king.pop();

	// last board state
	gboard = prev_brd_states.top();
	prev_brd_states.pop();

	// last piece list state
	const auto chng = prev_plist_changes.top();
	prev_plist_changes.pop();

	uint8_t index = (chng.cpy1 & (0b11111 << 8)) >> 8;
	gpiece_list[index] = chng.cpy1 & (0b11111111);

	if (!chng.second) {
		return;
	}

	index = (chng.cpy2 & (0b11111 << 8)) >> 8;
	gpiece_list[index] = chng.cpy2 & (0b11111111);
}


void AI::MoveTraits::SetActualEnPassant(const sf::Vector2i new_en_passant) noexcept {
	en_passant_pos = new_en_passant;
}


void AI::MoveTraits::SetKingsPos(const sf::Vector2i white_king, const sf::Vector2i black_king) {
	white_king_pos = white_king;
	black_king_pos = black_king;
}


bool AI::MoveTraits::CheckPromote() noexcept {
	return promote_flag;
}


sf::Vector2i AI::MoveTraits::GetPrevEnPassant() {
	return en_passant_pos;
}


sf::Vector2i AI::MoveTraits::GetKingPos(pt::PieceColor king_col) {
	return (king_col == pt::PieceColor::WHITE) ?
		white_king_pos : black_king_pos;
}


void AI::MoveTraits::UpdateBoard(pt::board_grid_t& gboard, const pt::PieceList& gpiece_list) {
	for (const auto& piece : gpiece_list) {
		if (pt::CheckUnValidity(piece)) {
			continue;
		}

		brdclass_ptr->SetPieceOccupiedFields(gboard, gpiece_list, pt::GetYPos(piece), pt::GetXPos(piece));
	}
}


void AI::MoveTraits::EnPassantCase(
	pt::board_grid_t& gboard, pt::PieceList& gpiece_list, const sf::Vector2i new_pos, const int8_t d, pdata_change& chng) {
	auto& moved_piece = gboard[new_pos.y][new_pos.x];

	// capturing pawn using en passant
	if (new_pos.y - d == en_passant_pos.y and new_pos.x == en_passant_pos.x) {
		const auto cap_index = pt::GetPieceListIndex(gboard[new_pos.y - d][new_pos.x]);
		chng.second = true;
		chng.cpy2 = ((cap_index << 8) | gpiece_list[cap_index]);
		pt::UnSetValidity(gpiece_list[cap_index]);
		gboard[new_pos.y - d][new_pos.x] &= (0b11111 << 7);
	}
	else { // setting en passant position

		const bool is_double_field_move =
			(pt::CheckColor(moved_piece, pt::BIT_WHITE) and new_pos.y == 4) or
			(pt::CheckColor(moved_piece, pt::BIT_BLACK) and new_pos.y == 3);

		if (is_double_field_move and !pt::CheckMoveTag(moved_piece)) {
			SetActualEnPassant(new_pos);
			return;
		}
	}

	pt::SetMoveTag(moved_piece);
	SetActualEnPassant({ -1, -1 });
}


void AI::MoveTraits::CastleCase(
	pt::board_grid_t& gboard, pt::PieceList& gpiece_list, const sf::Vector2i old_pos, sf::Vector2i new_pos, pdata_change& chng) {

	auto rook_index = pt::GetPieceListIndex(gboard[new_pos.y][new_pos.x]),
		king_index = pt::GetPieceListIndex(gboard[old_pos.y][old_pos.x]);

	chng.second = true;
	chng.cpy2 = ((rook_index << 8) | gpiece_list[rook_index]);
	chng.cpy1 = ((king_index << 8) | gpiece_list[king_index]);

	if (new_pos.x == 0) {
		gpiece_list.ChangePos(rook_index, 3, new_pos.y);

		gboard[new_pos.y][new_pos.x] = 0;

		new_pos = sf::Vector2i(1, new_pos.y);
		gboard[new_pos.y][new_pos.x] = gboard[old_pos.y][old_pos.x];

		gboard[new_pos.y][3] =
			pt::GetBitColor(gboard[new_pos.y][new_pos.x]) | pt::BIT_ROOK;
	}
	else {
		gpiece_list.ChangePos(rook_index, 5, new_pos.y);

		gboard[new_pos.y][new_pos.x] = 0;

		new_pos = sf::Vector2i(BOARD_SIZE - 2, new_pos.y);
		gboard[new_pos.y][new_pos.x] = gboard[old_pos.y][old_pos.x];

		gboard[new_pos.y][5] =
			pt::GetBitColor(gboard[new_pos.y][new_pos.x]) | pt::BIT_ROOK;
	}

	brdclass_ptr->ChangePiecePos(gboard, old_pos, new_pos);
	pt::SetMoveTag(gboard[new_pos.y][new_pos.x]);

	king_index = pt::GetPieceListIndex(gboard[new_pos.y][new_pos.x]);
	gpiece_list.ChangePos(king_index, new_pos);
}