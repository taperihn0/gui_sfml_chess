#include "Piece.h"

Piece::Piece(const sf::Vector2i& pos, const std::string& texture_path, Board* board_ptr,
	const uint16_t& size)
	: position(pos), board(board_ptr), first_move(true), piece_texture_path(texture_path) {
	PreparePieceTexture(size);
}


void Piece::PreparePieceTexture(const uint16_t& size) {
	piece_texture_ptr = std::make_shared<sf::Texture>();

	if (!piece_texture_ptr->loadFromFile(piece_texture_path)) {
		throw std::runtime_error("Error during loading piece texture from file.");
	}
	
	piece_texture_ptr->setSmooth(true);
	piece_sprite.setTexture(*piece_texture_ptr);

	const auto& curr_size = piece_sprite.getTextureRect();
	piece_sprite.setScale(float(size) / curr_size.width, float(size) / curr_size.height);
}


void Piece::DrawPiece() {
	board->DrawOnField(piece_sprite, position);
}


//void Piece::ShowPossibleFields() {
//
//}


void Piece::ChangePosition(const sf::Vector2i& new_pos) {
	position = new_pos;
}

sf::Vector2i Piece::GetPosition() {
	return position;
}