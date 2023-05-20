#pragma once

#include <SFML\Graphics.hpp>

#include <stdexcept>
#include <array>
#include <cstdint>
#include <utility>
#include <memory>

class Board {
public:
	Board(const uint16_t& window_size);
	void PrepareBoard();

	void DrawOnField(sf::Sprite& piece_sprite, sf::Vector2i& pos);

	sf::Sprite GetBoardSprite();
	uint16_t GetFieldSize();

	enum class PieceType {
		EMPTY = 0, PAWN, BISHOP, KNIGHT, ROOK, QUEEN, KING
	};

	enum class PieceColor {
		EMPTY = 0, WHITE, BLACK
	};

private:
	sf::RenderTexture render_board;
	sf::RenderTexture plain_board;
	const sf::Color light_field, dark_field;

	const uint16_t WINDOW_SIZE, FIELD_SIZE;
	static constexpr uint8_t BOARD_SIZE = 8;

	std::array<std::array<sf::Vector2f, BOARD_SIZE>, BOARD_SIZE> fields_coordinates;
	std::array<std::array<std::pair<PieceColor, PieceType>, 8>, 8> pieces_distrb;
};

