#pragma once

#include "..\PieceData.h"

#include <vector>

class Board;

class Piece {
public:
	Piece(
		const std::string& texture_path, Board* board_ptr,
		uint16_t size, pt::PieceColor piece_color_);

	// just load necessery resources from hard drive 
	void PreparePieceTexture(const uint16_t size);

	// draw piece's sprite on an transparent surface 
	void DrawPiece(sf::Vector2f& window_pos);

	// return vector of fields where piece can move
	virtual const std::vector<sf::Vector2i>&
	GetActiveFields(
		const pt::board_grid_t& gboard,
		sf::Vector2i pos, sf::Vector2i ep_pos, sf::Vector2i king_pos,
		const pt::PieceList& gpiece_list,
		bool consider_check = true, bool clear = true) = 0;

	// check all the occupied fields and it will be just set of active fields
	// for most of the time with some exceptions
	// mark them on the given board
	virtual void MarkOccupiedFields(
		pt::board_grid_t& gboard,
		sf::Vector2i ep_pos, sf::Vector2i king_pos,
		const pt::PieceList& gpiece_list, 
		sf::Vector2i pos);

	// check whether field is avaible - if it's valid field
	// or is not occupied by different piece
	bool CheckFieldFreeValid(
		const pt::board_grid_t& gboard, sf::Vector2i pos) noexcept;

	// check if field is valid and if it's occupied by enemy piece
	bool CheckFieldOccupied(
		const pt::board_grid_t& gboard,
		sf::Vector2i pos, pt::PieceColor piece_color) noexcept;

	// check whether given move of a piece block check
	bool CheckCheckSafe(
		pt::board_grid_t gboard_cpy,
		const pt::PieceList& gpiece_list,
		sf::Vector2i old_pos, sf::Vector2i new_pos);

	// set an occuper color of the given field
	void MarkSingleOccupied(pt::field_t& field) noexcept;

	// looping through given grid, generating occupied fields
	// and checking whether given condition is fulfilled
	template<typename T>
	bool LoopGenerateOccupied(pt::board_grid_t&& gboard, const pt::PieceList& gpiece_list, T&& break_condtn);

	virtual ~Piece() = default;
protected:
	sf::Sprite piece_sprite;
	std::vector<sf::Vector2i> avaible_fields;

	Board* const brdclass_ptr;

	const pt::PieceColor piece_color;

	const std::string piece_texture_path;
	const std::unique_ptr<sf::Texture> piece_texture_ptr;

	bool is_check;

	sf::Vector2i brd_ep;
	sf::Vector2i brd_king_pos;
};
