#pragma once

#include <SFML\Graphics.hpp>
#include "..\Board\Board.h"

#include <memory>
#include <vector>

class Piece {
public:
	Piece(const sf::Vector2i& pos, const std::string& texture_path, Board* board_ptr,
		const uint16_t& size);
	virtual void PreparePieceTexture(const uint16_t &size);
	
	//virtual void ShowPossibleFields();
	virtual void DrawPiece();
	virtual void ChangePosition(const sf::Vector2i& new_pos);

	virtual sf::Vector2i GetPosition();
protected:
	sf::Vector2i position;
	sf::Sprite piece_sprite;
	std::vector<sf::Vector2i> active_fields;
	
	Board* const board;

	bool first_move;

	const std::string piece_texture_path;
	std::shared_ptr<sf::Texture> piece_texture_ptr;
};

