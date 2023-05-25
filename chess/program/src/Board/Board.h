#pragma once

#include <SFML\Graphics.hpp>

#include "..\Indicator.h"
#include "..\Pawn\Pawn.h"
#include "..\Rook\Rook.h"
#include "..\Bishop\Bishop.h"
#include "..\Knight\Knight.h"
#include "..\Queen\Queen.h"
#include "..\King\King.h"

#include <iostream>
#include <stdexcept>
#include <array>
#include <cstdint>
#include <memory>
#include <vector>
#include <algorithm>
#include <utility>

class Board {
public:
	Board(const uint16_t& window_size, const bool& show_console_board_);
	void PreparePiecesTemplate();
	void PrepareBoard();
	void InitBoardFields() noexcept;

	void DrawOnPiecesSurfaceField(sf::Sprite& piece_sprite, sf::Vector2f& window_pos);

	sf::Sprite GetBoardSprite();
	const uint16_t& GetFieldSize() noexcept;

	void ProcessPressedMouse(const sf::Vector2i& mouse_pos);

	static bool isValidField(const sf::Vector2i& coords) noexcept;
private:
	void LocatePieceOnSurface(const uint8_t& row, const uint8_t& col);

	void FocusPieceField(const PieceFlags::Indicator& picked_piece, const sf::Vector2i& field_pos);
	void UnfocusPieceField(const sf::Vector2i& field_pos);
	void MovePiece(const sf::Vector2i& new_move_field) noexcept;

	bool isValidFocused() noexcept;

	std::pair<bool, sf::Vector2i> 
	CheckAndGetIfFocused(const sf::Vector2i& coords);

	void UpdateBoard();
	void UpdatePiecesSurface();

	sf::RenderTexture render_board;
	sf::RenderTexture plain_board;
	sf::RenderTexture pieces_surface;

	const sf::Color light_field, dark_field, highlighted_field;
	std::array<sf::Color, 2> grid_colors;

    const uint16_t WINDOW_SIZE, FIELD_SIZE;
	static constexpr uint8_t BOARD_SIZE = 8;

	std::array<std::array<sf::Vector2f, BOARD_SIZE>, BOARD_SIZE> fields_coordinates;
	std::array<std::array<PieceFlags::Indicator, 8>, 8> pieces_indicator;
	std::array<std::array<std::unique_ptr<Piece>, 12 + 1>, 2 + 1> pieces_templates;

	sf::Vector2i curr_focused_pos;
	std::vector<sf::Vector2i> active_focused_field;

	bool show_console_board;
};

