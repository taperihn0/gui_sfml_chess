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

// check all the occupied fields and
// it will be just set of active fields
// for most of the time with some exceptions
void Piece::MarkOccupiedFields(
	std::array<std::array<PieceFlags::Indicator, 8>, 8>& pieces_indicator,
	const sf::Vector2i& pos) {
	const auto active_fields(GetActiveFields(pieces_indicator, pos));

	for (const auto& occupied_field : active_fields) {
		MarkSingleOccupied(pieces_indicator[occupied_field.y][occupied_field.x]);
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

// set given field as occupied by color of this piece
void Piece::MarkSingleOccupied(PieceFlags::Indicator& field) noexcept {
	if (piece_color == PieceFlags::PieceColor::WHITE) {
		field.occuping_color.white = true;
	}
	else {
		field.occuping_color.black = true;
	}
}