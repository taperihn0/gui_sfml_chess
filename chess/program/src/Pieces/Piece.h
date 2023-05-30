#pragma once

#include <SFML\Graphics.hpp>

#include "..\Indicator.h"

#include <memory>
#include <vector>

class Board;

class Piece {
public:
	Piece(const std::string& texture_path, Board* board_ptr,
		const uint16_t& size, const PieceFlags::PieceColor& piece_color_);

	virtual void PreparePieceTexture(const uint16_t &size);
	
	virtual void DrawPiece(sf::Vector2f& window_pos);

	virtual std::vector<sf::Vector2i>&& GetActiveFields(
		const std::array<std::array<PieceFlags::Indicator, 8>, 8>& pieces_indicator,
		const sf::Vector2i& pos, const bool& clear = true) = 0;

	virtual void MarkOccupiedFields(
		std::array<std::array<PieceFlags::Indicator, 8>, 8>& pieces_indicator,
		const sf::Vector2i& pos
	);

	bool CheckFieldFreeValid(
		const std::array<std::array<PieceFlags::Indicator, 8>, 8>& pieces_indicator,
		const sf::Vector2i& pos) noexcept;

	bool CheckFieldOccupied(
		const std::array<std::array<PieceFlags::Indicator, 8>, 8>& pieces_indicator,
		const sf::Vector2i& pos, const PieceFlags::PieceColor& piece_color) noexcept;

	virtual ~Piece() = default;
protected:
	void MarkSingleOccupied(PieceFlags::Indicator& field) noexcept;

	sf::Sprite piece_sprite;
	std::vector<sf::Vector2i> avaible_fields;
	
	Board* const board;

	const PieceFlags::PieceColor piece_color;

	const std::string piece_texture_path;
	const std::unique_ptr<sf::Texture> piece_texture_ptr;
};

