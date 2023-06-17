#include "Piece.h"
#include "..\Board\Board.h"

Piece::Piece(const std::string& texture_path, Board* board_ptr,
	const uint16_t& size, const PieceFlags::PieceColor& piece_color_)
	: brdclass_ptr(board_ptr),
	piece_color(piece_color_), 
	piece_texture_path(texture_path),
	piece_texture_ptr(std::make_unique<sf::Texture>()) {
	PreparePieceTexture(size);
}


void Piece::PreparePieceTexture(const uint16_t& size) {
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


bool Piece::CheckFieldFreeValid(
	const PieceFlags::board_grid_t& pieces_indicator,
	const sf::Vector2i& pos) noexcept {
	return Board::isValidField(pos) and
		pieces_indicator[pos.y][pos.x].type == PieceFlags::PieceType::EMPTY;
}


bool Piece::CheckCheckSafe(
	PieceFlags::board_grid_t pieces_indicator_cpy,
	sf::Vector2i old_pos, sf::Vector2i new_pos) {

	brdclass_ptr->ZeroEntireBoardOccuperColor(pieces_indicator_cpy);

	// simulation of piece's move - if the move won't cause check on king of 
	// same color as a moving piece, then the move is unvalid
	// Just moving piece on a copied board and checking if king is under attack
	brdclass_ptr->ChangePiecePos(pieces_indicator_cpy, old_pos, new_pos);

	// if the king is already attacked, there is no sense to continue
	// generating new occupied fields - king is attacked anyway and it won't change

	return LoopGenerateOccupied(std::move(pieces_indicator_cpy), [this](const PieceFlags::board_grid_t& board) {
		return brdclass_ptr->CheckKingAttacked(board, piece_color, brd_king_pos);
	});
}


void Piece::MarkOccupiedFields(
	PieceFlags::board_grid_t& board,
	const sf::Vector2i& pos, bool consider_check) {

	// occupied fields are just active fields of some of the pieces
	// for most of the time
	auto&& active_fields = GetActiveFields(board, pos, consider_check);

	for (const auto& occupied_field : active_fields) {
		MarkSingleOccupied(board[occupied_field.y][occupied_field.x]);
	}
}


bool Piece::CheckFieldOccupied(
	const PieceFlags::board_grid_t& pieces_indicator,
	const sf::Vector2i& pos, const PieceFlags::PieceColor& piece_color_) noexcept {
	return Board::isValidField(pos) and
		pieces_indicator[pos.y][pos.x].type != PieceFlags::PieceType::EMPTY and
		pieces_indicator[pos.y][pos.x].color != piece_color_;
}


void Piece::MarkSingleOccupied(PieceFlags::Indicator& field) noexcept {
	if (piece_color == PieceFlags::PieceColor::WHITE) {
		field.occuping_color.white = true;
	}
	else {
		field.occuping_color.black = true;
	}
}


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