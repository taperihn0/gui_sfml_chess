#include "Piece.h"
#include "..\Board\Board.h"

Piece::Piece(const std::string& texture_path, Board* board_ptr,
	uint16_t size, pt::PieceColor piece_color_)
	: brdclass_ptr(board_ptr),
	piece_color(piece_color_), 
	piece_texture_path(texture_path),
	piece_texture_ptr(std::make_unique<sf::Texture>()) {
	PreparePieceTexture(size);
}


void Piece::PreparePieceTexture(const uint16_t size) {
	if (!piece_texture_ptr->loadFromFile(piece_texture_path)) {
		throw std::runtime_error("Error during loading piece texture from file.");
	}
	
	piece_texture_ptr->setSmooth(true);
	piece_sprite.setTexture(*piece_texture_ptr);

	const auto& curr_size(piece_sprite.getTextureRect());
	piece_sprite.setScale(static_cast<float>(size) / curr_size.width, static_cast<float>(size) / curr_size.height);
}


void Piece::DrawPiece(sf::Vector2f& window_pos) {
	brdclass_ptr->DrawOnPiecesSurfaceField(piece_sprite, window_pos);
}


bool Piece::CheckCheckSafe(
	pt::board_grid_t gboard_cpy,
	const pt::PieceList& gpiece_list,
	sf::Vector2i old_pos, sf::Vector2i new_pos) {

	brdclass_ptr->ZeroEntireBoardOccuperColor(gboard_cpy);

	// simulation of piece's move - if the move won't cause check on king of 
	// same color as a moving piece, then the move is unvalid
	// just moving piece on a copied board and checking if king is under attack

	// if statement below producted just to prevent a situation when the piece is
	// changing positions and it is capturing another enemy piece
	// we need to ignore such captured piece when looping through piece list

	uint8_t index_cpy;
	bool list_change(false);

	if (!pt::CheckType(gboard_cpy[new_pos.y][new_pos.x], pt::BIT_EMPTY)) {
		list_change = true;
		index_cpy = pt::GetPieceListIndex(gboard_cpy[new_pos.y][new_pos.x]);

		// getting rid of const trait of list of piece because i'm too lazy
		// to change all of the function declarations

		pt::UnSetValidity(
			const_cast<pt::PieceList&>(gpiece_list)[pt::GetPieceListIndex(gboard_cpy[new_pos.y][new_pos.x])]);
	}

	brdclass_ptr->ChangePiecePos(gboard_cpy, old_pos, new_pos);

	// don't have to change pos of the piece in piece list, because i will look for 
	// pieces of different color

	// if the king is already attacked, there is no sense to continue
	// generating new occupied fields - king is attacked anyway and it won't change

	const bool res = LoopGenerateOccupied(std::move(gboard_cpy), gpiece_list, 
		[this](const pt::board_grid_t& gboard) {
		return brdclass_ptr->CheckKingAttacked(gboard, piece_color, brd_king_pos);
	});

	if (list_change) {
		pt::SetValidity(
			const_cast<pt::PieceList&>(gpiece_list)[index_cpy] &= ~(0b1 << 7));
	}

	return res;
}


void Piece::MarkOccupiedFields(
	pt::board_grid_t& gboard,
	sf::Vector2i ep_pos, sf::Vector2i king_pos,
	const pt::PieceList& gpiece_list,
	sf::Vector2i pos) {

	// occupied fields are just active fields of some of the pieces
	// for most of the time

	for (const auto& occupied_field : GetActiveFields(gboard, pos, ep_pos, king_pos, gpiece_list, false)) {
		MarkSingleOccupied(gboard[occupied_field.y][occupied_field.x]);
	}
}


bool Piece::CheckFieldFreeValid(
	const pt::board_grid_t& gboard, sf::Vector2i pos) noexcept {
	return Board::isValidField(pos) and pt::CheckType(gboard[pos.y][pos.x], pt::BIT_EMPTY);
}


bool Piece::CheckFieldOccupied(
	const pt::board_grid_t& gboard,
	sf::Vector2i pos, pt::PieceColor piece_color_) noexcept {
	return
		Board::isValidField(pos) and
		!pt::CheckType(gboard[pos.y][pos.x], pt::BIT_EMPTY) and
		!pt::CheckColor(gboard[pos.y][pos.x], pt::CastToBitColor(piece_color_));
}


void Piece::MarkSingleOccupied(pt::field_t& field) noexcept {
	pt::SetOccuper(field, piece_color);
}


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