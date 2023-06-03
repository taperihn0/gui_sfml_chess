#include "Piece.h"
#include "..\Board\Board.h"

Piece::Piece(const std::string& texture_path, Board* board_ptr,
	const uint16_t& size, const PieceFlags::PieceColor& piece_color_)
	: board(board_ptr), piece_color(piece_color_), piece_texture_path(texture_path),
	piece_texture_ptr(std::make_unique<sf::Texture>()) {
	PreparePieceTexture(size);
}

// just load necessery resources from hard drive 
void Piece::PreparePieceTexture(const uint16_t& size) {
	if (!piece_texture_ptr->loadFromFile(piece_texture_path)) {
		throw std::runtime_error("Error during loading piece texture from file.");
	}
	
	piece_texture_ptr->setSmooth(true);
	piece_sprite.setTexture(*piece_texture_ptr);

	const auto& curr_size = piece_sprite.getTextureRect();
	piece_sprite.setScale(float(size) / curr_size.width, float(size) / curr_size.height);
}

// draw piece's sprite on an transparent surface 
void Piece::DrawPiece(sf::Vector2f& window_pos) {
	board->DrawOnPiecesSurfaceField(piece_sprite, window_pos);
}

// check whether field is avaible - if it's valid field
// or is not occupied by different piece
bool Piece::CheckFieldFreeValid(
	const std::array<std::array<PieceFlags::Indicator, 8>, 8>& pieces_indicator,
	const sf::Vector2i& pos) noexcept {
	return Board::isValidField(pos) and
		pieces_indicator[pos.y][pos.x].type == PieceFlags::PieceType::EMPTY;
}

// check whether given move of a piece block mate
bool Piece::CheckMateSafe(
	std::array<std::array<PieceFlags::Indicator, 8>, 8> pieces_indicator_cpy,
	sf::Vector2i old_pos, sf::Vector2i new_pos) {

	// prepare board for simulation
	board->ZeroEntireBoardOccuperColor(pieces_indicator_cpy);

	// simulation of piece's move - if the move won't cause mate on king of 
	// same color as a moving piece, then the move is unvalid
	// Just moving piece on a copied board and checking if king is under attack
	board->ChangePiecePos(pieces_indicator_cpy, old_pos, new_pos);

	for (uint8_t i = 0; i < 8; i++) {
		for (uint8_t j = 0; j < 8; j++) {
			if (pieces_indicator_cpy[i][j].type != PieceFlags::PieceType::EMPTY and
				pieces_indicator_cpy[i][j].color != piece_color) {
				board->SetPieceOccupiedFields(pieces_indicator_cpy, pieces_indicator_cpy[i][j], i, j, false);
			}
		}
	}

	return !board->CheckKingAttacked(pieces_indicator_cpy, piece_color);
}

// check all the occupied fields and it will be just set of active fields
// for most of the time with some exceptions
// mark them on the given board
void Piece::MarkOccupiedFields(
	std::array<std::array<PieceFlags::Indicator, 8>, 8>& board,
	const sf::Vector2i& pos, bool consider_mate) {
	const auto active_fields(GetActiveFields(board, pos, consider_mate));

	for (const auto& occupied_field : active_fields) {
		MarkSingleOccupied(board[occupied_field.y][occupied_field.x]);
	}
}

// check if field is valid and if it's occupied by enemy piece
bool Piece::CheckFieldOccupied(
	const std::array<std::array<PieceFlags::Indicator, 8>, 8>& pieces_indicator,
	const sf::Vector2i& pos, const PieceFlags::PieceColor& piece_color_) noexcept {
	return Board::isValidField(pos) and
		pieces_indicator[pos.y][pos.x].type != PieceFlags::PieceType::EMPTY and
		pieces_indicator[pos.y][pos.x].color != piece_color_;
}

// set an occuper color of the given field
void Piece::MarkSingleOccupied(PieceFlags::Indicator& field) noexcept {
	if (piece_color == PieceFlags::PieceColor::WHITE) {
		field.occuping_color.white = true;
	}
	else {
		field.occuping_color.black = true;
	}
}