#include "Piece.h"
#include "..\Board\Board.h"

Piece::Piece(const std::string& texture_path, Board* board_ptr,
	const uint16_t& size)
	: board(board_ptr), first_move(true), piece_texture_path(texture_path) {
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


void Piece::DrawPiece(sf::Vector2f& window_pos) {
	board->DrawOnSurfaceField(piece_sprite, window_pos);
}