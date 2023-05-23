#pragma once

#include <SFML\Graphics.hpp>

#include "..\Pawn\Pawn.h"
#include "..\Rook\Rook.h"
#include "..\Bishop\Bishop.h"
#include "..\Knight\Knight.h"
#include "..\Queen\Queen.h"
#include "..\King\King.h"

// <iostream> library just for amateur debuging purposes
#include <iostream>
#include <stdexcept>
#include <array>
#include <cstdint>
#include <memory>

class Board {
public:
	Board(const uint16_t& window_size);
	void PreparePiecesTemplate();
	void PrepareBoard();
	void InitBoardFields() noexcept;

	void DrawOnSurfaceField(sf::Sprite& piece_sprite, sf::Vector2f& window_pos);

	void UpdateBoard();
	sf::Sprite GetBoardSprite();
	const uint16_t& GetFieldSize() noexcept;

	void FocusField(const sf::Vector2i &mouse_pos);

	enum class PieceType {
		EMPTY = 0, PAWN, BISHOP, KNIGHT, ROOK, QUEEN, KING
	};

	enum class PieceColor {
		EMPTY = 0, WHITE, BLACK
	};

private:
	void LocatePieceOnSurface(const uint8_t& row, const uint8_t& col);
	
	bool isValidField(const sf::Vector2i &coords) noexcept;

	struct Indicator {
		PieceColor color;
		PieceType type;
	};

	sf::RenderTexture render_board;
	sf::RenderTexture plain_board;
	sf::RenderTexture pieces_surface;
	const sf::Color light_field, dark_field, highlighted_field;

    const uint16_t WINDOW_SIZE, FIELD_SIZE;
	static constexpr uint8_t BOARD_SIZE = 8;

	std::array<std::array<sf::Vector2f, BOARD_SIZE>, BOARD_SIZE> fields_coordinates;
	std::array<std::array<Indicator, 8>, 8> pieces_indicator;
	std::array<std::array<std::unique_ptr<Piece>, 12 + 1>, 2 + 1> pieces_templates;
};

