#pragma once

#include <SFML\Graphics.hpp>

#include <memory>
#include <vector>

class Board;

class Piece {
public:
	Piece() = default;
	Piece(const std::string& texture_path, Board* board_ptr,
		const uint16_t& size);
	virtual void PreparePieceTexture(const uint16_t &size);
	
	virtual void DrawPiece(sf::Vector2f& window_pos);
protected:
	sf::Sprite piece_sprite;
	std::vector<sf::Vector2i> active_fields;
	
	Board* const board;

	bool first_move;

	const std::string piece_texture_path;
	std::shared_ptr<sf::Texture> piece_texture_ptr;
};

