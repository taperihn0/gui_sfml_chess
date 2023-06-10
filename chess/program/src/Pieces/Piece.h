#pragma once

#include <SFML\Graphics.hpp>

#include "..\Indicator.h"

#include <vector>

class Board;

class Piece {
public:
	Piece(const std::string& texture_path, Board* board_ptr,
		const uint16_t& size, const PieceFlags::PieceColor& piece_color_);

	// just load necessery resources from hard drive 
	void PreparePieceTexture(const uint16_t &size);
	
	// draw piece's sprite on an transparent surface 
	void DrawPiece(sf::Vector2f& window_pos);

	virtual std::vector<sf::Vector2i>&& GetActiveFields(
		const PieceFlags::board_grid_t& pieces_indicator,
		const sf::Vector2i& pos, bool consider_check = true, bool clear = true) = 0;

	// check all the occupied fields and it will be just set of active fields
	// for most of the time with some exceptions
	// mark them on the given board
	virtual void MarkOccupiedFields(
		PieceFlags::board_grid_t& board,
		const sf::Vector2i& pos, bool consider_check = true);

	// check whether field is avaible - if it's valid field
	// or is not occupied by different piece
	bool CheckFieldFreeValid(
		const PieceFlags::board_grid_t& pieces_indicator,
		const sf::Vector2i& pos) noexcept;

	// check if field is valid and if it's occupied by enemy piece
	bool CheckFieldOccupied(
		const PieceFlags::board_grid_t& pieces_indicator,
		const sf::Vector2i& pos, const PieceFlags::PieceColor& piece_color) noexcept;

	// check whether given move of a piece block check
	bool CheckCheckSafe(
		PieceFlags::board_grid_t pieces_indicator_cpy,
		sf::Vector2i old_pos, sf::Vector2i new_pos);

	virtual ~Piece() = default;
protected:
	// set an occuper color of the given field
	void MarkSingleOccupied(PieceFlags::Indicator& field) noexcept;

	sf::Sprite piece_sprite;
	std::vector<sf::Vector2i> avaible_fields;
	
	Board* const board;

	const PieceFlags::PieceColor piece_color;

	const std::string piece_texture_path;
	const std::unique_ptr<sf::Texture> piece_texture_ptr;

	bool is_check;
};

