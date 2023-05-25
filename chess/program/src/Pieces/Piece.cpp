#include "Piece.h"
#include "..\Board\Board.h"

Piece::Piece(const std::string& texture_path, Board* board_ptr,
	const uint16_t& size, const PieceFlags::PieceColor& piece_color_)
	: avaible_fields{}, board(board_ptr), piece_color(piece_color_), piece_texture_path(texture_path) {
	PreparePieceTexture(size);
}

// just load necessery resources from hard drive 
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

// draw piece sprite on an alpha surface 
void Piece::DrawPiece(sf::Vector2f& window_pos) {
	board->DrawOnPiecesSurfaceField(piece_sprite, window_pos);
}
