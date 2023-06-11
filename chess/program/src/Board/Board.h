#pragma once

#include <SFML\Graphics.hpp>
#include <SFML\Audio.hpp>

#include "..\Indicator.h"
#include "..\Pawn\Pawn.h"
#include "..\Rook\Rook.h"
#include "..\Bishop\Bishop.h"
#include "..\Knight\Knight.h"
#include "..\Queen\Queen.h"
#include "..\King\King.h"

#include "..\AI\Engine.h"

#include <iostream>
#include <stdexcept>
#include <cstdint>
#include <vector>
#include <algorithm>

class Board {
public:
	Board(const uint16_t& window_size, const bool& show_console_board_);

	// arrange starting positions of pieces for both players
	void PrepareBoard();

	// draw a piece on his special surface
	void DrawOnPiecesSurfaceField(sf::Sprite& piece_sprite, sf::Vector2f& window_pos);

	// return real sprite of entire board with pieces
	sf::Sprite GetBoardSprite();

	// size in pixels of a single field on the board
	const uint16_t& GetFieldSize() noexcept;

	// size of a chess board
	static constexpr uint8_t GetBoardSize() noexcept;

	// return current field, which contain pawn
	// which is possible to capture by en passant technique - 
	// there is only one such pawn on the board
	const sf::Vector2i& GetEnPassantPos() noexcept;

	// process every single click in the area of game window
	void ProcessPressedMouse(const sf::Vector2i& mouse_pos);

	// zero occuper color of field
	void ZeroEntireBoardOccuperColor(PieceFlags::board_grid_t& board);

	// set all of the piece occupied fields
	void SetPieceOccupiedFields(
		PieceFlags::board_grid_t& board, const uint8_t& y, const uint8_t& x, bool consider_mate = false);

	// simply move a piece and change his position in given 2d array
	void ChangePiecePos(
		PieceFlags::board_grid_t& board,
		sf::Vector2i old_pos, sf::Vector2i new_pos) noexcept;

	// kind of ChangePiecePos() function, but for king and castling
	void CastleKingChange(
		PieceFlags::board_grid_t& board,
		sf::Vector2i old_pos, sf::Vector2i new_pos);

	// all the problems with en passant capture in one function -
	// capturing and updating current pawn which can be captured using en passant technique
	// return updated en passant pos
	sf::Vector2i EnPassantCase(
		PieceFlags::board_grid_t& board, const sf::Vector2i new_pos, const PieceFlags::Indicator moved_piece, const int8_t d);

	// check whether given coordinates are valid for my board
	static bool isValidField(const sf::Vector2i& coords) noexcept;

	// is there a mate on a king of a given color?
	bool CheckKingAttacked(
		const PieceFlags::board_grid_t& board,
		PieceFlags::PieceColor king_color) noexcept;

	// return true whether game is over
	bool CheckForMateStealMate();
private:
	struct FieldDataFlag;

	// fill fields with starting postitions of pieces
	void InitBoardFields() noexcept;

	// generate template for every single piece
	void PreparePiecesTemplate();

	// load all the needed sounds from hard drive
	void PrepareAudio();

	// for every piece in 2d array
	// locate it in a proper field in alpha surface
	// and generate its occupied fields
	void LocatePieceOnSurface(const uint8_t& y, const uint8_t& x);

	// generating all the possible moves by player who just got a turn
	// and storing it in cache memory
	// if the player has no possible moves left, game is over
	/*uint16_t PreGenerateAllMoves();*/

	// focus after clicking on a piece
	void FocusPieceField(const PieceFlags::Indicator& picked_piece, const sf::Vector2i& field_pos);

	// unfocus after clicking exactly on a focused piece
	void UnfocusPieceField(const sf::Vector2i& field_pos);

	// move given piece to a given new field - 
	// occupy empty field or capture enemy piece there
	void MovePiece(const sf::Vector2i old_pos, const sf::Vector2i new_pos);

	// check whether my focus flag is set
	bool isValidFocused() noexcept;

	// check whether field is in focus fields
	FieldDataFlag
		CheckAndGetIfFocused(const sf::Vector2i& coords);

	// check whether given color is the same as current color of moving player
	bool CheckCurrTurnColor(const PieceFlags::PieceColor& color) noexcept;

	// check if king moved and update its position
	void CheckUpdateIfKingMove(sf::Vector2i new_pos);

	// draw window with pieces when pawn is upgrading
	void OpenPawnUpgradeWindow(const sf::Vector2i& pos);

	// process mouse click - 
	// if it's not valid, just wait for 
	// proper click pos and leave window turned on
	void PickPieceOnWindow(const sf::Vector2i& pos);

	// updating render_board sprite
	void UpdateBoard();

	// update pieces on their alpha surface
	// and set occupied fields of each piece on the surface
	void UpdatePiecesSurface();

	// change is_white_turn variable to indicate 
	// whose is turn now
	void ChangePlayersTurn() noexcept;

	// reset current en passant position
	void SetEnPassantPos(const sf::Vector2i new_pos) noexcept;

	// while any piece is moving or simply capturing
	void SetMoveSound(sf::Vector2i new_pos) noexcept;

	void SetCastleSound() noexcept;

	void SetEnPassantSound(sf::Vector2i pos) noexcept;

	struct FieldDataFlag {
		bool is_found;
		sf::Vector2i active_clicked;
	};

	sf::RenderTexture render_board;
	sf::RenderTexture plain_board;
	sf::RenderTexture pieces_surface;

	const sf::Color light_field,
		dark_field,
		highlighted_field,
		upgrade_window_color;

	std::array<sf::Color, 2> grid_colors;

	const uint16_t WINDOW_SIZE, FIELD_SIZE;

	std::array<std::array<sf::Vector2f, BOARD_SIZE>, BOARD_SIZE> fields_coordinates;
	PieceFlags::templates_t pieces_templates;
	PieceFlags::board_grid_t pieces_indicator;

	sf::Vector2i en_passant_pos,
		curr_focused_pos,
		black_king_pos, white_king_pos;
	std::vector<sf::Vector2i> active_focused_field;

	std::array<PieceFlags::PieceType, 4> list_of_window_pieces;

	const bool show_console_board;

	bool is_pawn_upgrade_window;
	PieceFlags::PieceColor upgrading_color;
	uint8_t upgrading_x_pos;

	bool is_white_turn;

	PieceFlags::av_moves_board_t cache;
	uint16_t possible_moves;

	struct Audio {
		sf::Sound move,
			capture,
			castle,
			game_end,
			check,
			promote;
	} sounds;

	std::array<std::unique_ptr<sf::SoundBuffer>, 6> sbuffers;
	sf::Sound turn_sound;
	bool is_turn_sound;

	AI::Engine engine;
};