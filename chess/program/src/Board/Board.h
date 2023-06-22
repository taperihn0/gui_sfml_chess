#pragma once

#include <SFML\Audio.hpp>

#include "..\PieceData.h"
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
#include <cctype>

class Board {
public:
	Board(const uint16_t& window_size);

	// arrange starting positions of pieces for both players
	void PrepareBoard();

	// draw a piece on his special surface
	void DrawOnPiecesSurfaceField(sf::Sprite& piece_sprite, sf::Vector2f& window_pos);

	// return real sprite of entire board with pieces
	sf::Sprite GetBoardSprite();

	// size in pixels of a single field on the board
	inline uint16_t GetFieldSize() noexcept;

	// process every single click in the area of game window
	void ProcessPressedMouse(const sf::Vector2i& mouse_pos);

	// zero occuper color of field
	void ZeroEntireBoardOccuperColor(pt::board_grid_t& gboard);

	// set all of the occupied fields of piece of given pos in a gboard
	void SetPieceOccupiedFields(
		pt::board_grid_t& gboard, const pt::PieceList& gpiece_list, uint8_t y, uint8_t x);

	// simply move a piece and change his position in given 2d array
	void ChangePiecePos(
		pt::board_grid_t& gboard,
		const sf::Vector2i old_pos, const sf::Vector2i new_pos) noexcept;

	// kind of ChangePiecePos() function, but for king and castling
	void CastleKingChange(
		pt::board_grid_t& gboard,
		sf::Vector2i old_pos, sf::Vector2i& new_pos);

	// all the problems with en passant capture in one function -
	// capturing and updating current pawn which can be captured using en passant technique
	// return updated en passant pos
	sf::Vector2i EnPassantCase(
		pt::board_grid_t& gboard, const sf::Vector2i new_pos, 
		const int8_t d, const int8_t en_passant_y);

	// check if king moved and update its position
	void CheckUpdateIfKingMove(
		const pt::board_grid_t& gboard, const sf::Vector2i new_pos,
		sf::Vector2i& white_king, sf::Vector2i& black_king);

	// check whether given coordinates are valid for my board
	static bool isValidField(sf::Vector2i pos) noexcept;

	// is there a mate on a king of a given color?
	inline bool CheckKingAttacked(
		const pt::board_grid_t& gboard,
		const pt::PieceColor king_color, const sf::Vector2i check_pos) noexcept;

	// return true whether game is over
	bool CheckForMateStealMate();
private:
	struct FieldDataFlag;

	// fill fields with some FEN string
	void InitBoardFields() noexcept;

	// generate template for every single piece
	void PreparePiecesTemplate();

	// load all the needed sounds from hard drive
	void PrepareAudio();

	// for every piece in 2d array
	// locate it in a proper field in alpha surface
	// and generate its occupied fields
	void LocatePieceOnSurface(const uint8_t y, const uint8_t x);

	// focus after clicking on a piece
	void FocusPieceField(const pt::field_t& picked_piece, const sf::Vector2i& field_pos);

	// unfocus after clicking exactly on a focused piece
	void UnfocusPieceField(const sf::Vector2i& field_pos);

	// move given piece to a given new field - 
	// occupy empty field or capture enemy piece there
	void MovePiece(const sf::Vector2i old_pos, sf::Vector2i new_pos, const bool bot_turn = false);

	// check whether my focus flag is set
	inline bool isValidFocused() noexcept;

	// check whether field is in focus fields
	FieldDataFlag
	CheckAndGetIfFocused(sf::Vector2i pos);

	// check whether given color is the same as current color of moving player
	inline bool CheckCurrTurnColor(pt::PieceColor color) noexcept;

	// draw window with pieces when pawn is upgrading
	void OpenPawnUpgradeWindow(sf::Vector2i pos);

	// process mouse click - 
	// if it's not valid, just wait for 
	// proper click pos and leave window turned on
	void PickPieceOnWindow(sf::Vector2i pos);

	// updating render_board sprite
	void UpdateBoard();

	// update pieces on their alpha surface
	// and set occupied fields of each piece on the surface
	void UpdatePiecesSurface();

	// change is_white_turn variable to indicate 
	// whose is turn now
	inline void ChangePlayersTurn() noexcept;

	// reset current en passant position
	// it's a position of a pawn
	// which is possible to capture using en passant technique - 
	// there is only one such pawn on the board
	inline void SetEnPassantPos(const sf::Vector2i new_pos) noexcept;

	// reset current focused pos
	inline void SetFocusedPos(const sf::Vector2i new_pos) noexcept;

	// while any piece is moving or simply capturing
	void SetMoveSound(sf::Vector2i new_pos) noexcept;

	inline void SetCastleSound() noexcept;

	void SetEnPassantSound(sf::Vector2i pos) noexcept;

	struct FieldDataFlag {
		bool is_found;
		sf::Vector2i active_clicked;
	};

	sf::RenderTexture 
		render_board,
		plain_board,
		pieces_surface;

	const sf::Color 
		light_field,
		dark_field,
		highlighted_field,
		upgrade_window_color;

	std::array<sf::Color, 2> grid_colors;

	const uint16_t WINDOW_SIZE, FIELD_SIZE;

	std::array<std::array<sf::Vector2f, BOARD_SIZE>, BOARD_SIZE> fields_coordinates;
	pt::templates_t pieces_templates;
	pt::board_grid_t board;
	pt::PieceList piece_list;

	sf::Vector2i 
		en_passant_pos,
		curr_focused_pos,
		black_king_pos, white_king_pos;
	std::vector<sf::Vector2i> active_focused_field;

	std::array<pt::PieceType, 4> list_of_window_pieces;

	bool is_pawn_upgrade_window;
	pt::PieceColor upgrading_color;
	uint8_t upgrading_x_pos;

	bool is_white_turn;

	pt::av_moves_board_t cache;
	uint16_t possible_moves;

	struct Audio {
		sf::Sound 
			move,
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