#pragma once

#include "PieceList.h"

/* 
 structure of a field data: (field_t)
	[12 needed bits and 4 bits fields]
	uint8_t: |  00000  |  0  |  00  |  0  |  000  |
			     (1)     (2)    (3)   (4)    (5)
	
	1) index of the piece in piece list array
	2) move tag - 0 if no moves performed so far, or 1 if 1 or more moves (except pawns)
	3) field occuper color - 01 if it is white, else if 10 -  black, if 00 - no occuper
	4) color of the standing piece - 0 if white, else black
	5) piece type - can be 0 to indicate the field is empty


 structure of a piece data: (piece_data_t)
	[7 needed bits and 3 bits fields]
	uint8_t: |  0  |  000  |  000  |  0  |
			   (1)    (2)     (3)    (4) 

	1) tag of validity of a piece_data_t type(alias) - 
		- set as 0 by default
		- changing to 1 when subordinate piece is captured
	2) x coordinate of a same piece on a board
	3) y coordinate -||-
	4) color of the piece
*/


constexpr uint8_t BOARD_SIZE = 8;

class Piece;

namespace PieceTraits {
	enum class PieceType : uint8_t {
		EMPTY = 0, PAWN = 1, BISHOP = 2, KNIGHT = 3, ROOK = 4, QUEEN = 5, KING = 6
	};

	enum class PieceColor : uint8_t {
		WHITE = 0, BLACK = 1,
	};

	// some constans for piece_data_t and field_t types
	constexpr uint8_t
		BIT_EMPTY = 0,
		BIT_PAWN = 1,
		BIT_BISHOP = 2,
		BIT_KNIGHT = 3,
		BIT_ROOK = 4,
		BIT_QUEEN = 5,
		BIT_KING = 6,

		BIT_WHITE = 0b0 << 3,
		BIT_BLACK = 0b1 << 3,

		BIT_OCC_WHITE = 0b01 << 4,
		BIT_OCC_BLACK = 0b10 << 4,
		BIT_OCC_EMPTY = 0b00 << 4;

	constexpr bool
		BIT_PDATA_WHITE = 0b0,
		BIT_PDATA_BLACK = 0b1;

	// field structure alias
	using field_t = uint16_t;

	// handy aliases
	using board_grid_t = std::array<std::array<field_t, BOARD_SIZE>, BOARD_SIZE>;
	using av_moves_board_t = std::array<std::vector<sf::Vector2i>, PieceList::SIZE>;
	using templates_t = std::array<std::array<std::unique_ptr<Piece>, 12 + 1>, 2 + 1>;
	
	// returns true if given field has got same type as given one
	// type has to be uint8_t bit field type
	inline bool CheckType(const field_t field, const uint8_t type) {
		return (0b111 & field) == type;
	}

	// returns true if given field got same color as given one
	// type has to be uint8_t bit field type once again
	inline bool CheckColor(const field_t field, const uint8_t col) {
		return ((0b1 << 3) & field) == col;
	}

	// return type of a given field
	inline PieceType GetType(const field_t field) {
		return static_cast<PieceType>(0b111 & field);
	}

	// get color of given field (have not to be empty)
	inline PieceColor GetColor(const field_t field) {
		return static_cast<PieceColor>(static_cast<bool>(field & (0b1 << 3)));
	}

	// get index for piece list of given field
	inline uint8_t GetPieceListIndex(const field_t field) {
		return (field & (0b11111 << 7)) >> 7;
	}
	
	// get color as bit field
	inline uint8_t GetBitColor(const field_t field) {
		return field & (0b1 << 3);
	}

	// get bit type from PieceType object
	inline uint8_t CastToBitType(const PieceType type) {
		return static_cast<uint8_t>(type);
	}

	// get bit color from PieceColor object
	inline uint8_t CastToBitColor(const PieceColor col) {
		return static_cast<uint8_t>(col) << 3;
	}

	// delete, if any, occupers of a field
	inline void ZeroOccupers(field_t& field) {
		field &= ~(0b11 << 4);
	}

	// set occuper of a passed field of a given color
	inline void SetOccuper(field_t& field, const PieceColor col) {
		uint8_t tmp((col == PieceColor::WHITE) ? (0b01 << 4) : (0b10 << 4));
		field &= ~(0b11 << 4);
		field |= tmp;
	}

	// return current occuper as an uint8_t type
	inline uint8_t CheckOccuper(const field_t field, const uint8_t occ_col) {
		return (field & (0b11 << 4)) == occ_col;
	}

	// set move tag
	inline void SetMoveTag(field_t& field) {
		field |= (0b1 << 6);
	}

	// reset move tag of passed field
	inline void ZeroMoveTag(field_t& field) {
		field &= (0b0 << 6);
	}

	// return true if there where more or equal 1 move, else false
	inline bool CheckMoveTag(const field_t field) {
		return field & (0b1 << 6);
	}

	// return true if passed piece has same color as given color
	inline bool CheckPieceColor(const piece_data_t piece, const PieceColor col) {
		return (piece & 0b1) == static_cast<bool>(col);
	}

	// return x coordinate on the board surface of a piece
	inline uint8_t GetXPos(const piece_data_t piece) {
		return (piece & (0b111 << 4)) >> 4;
	}

	// return y coordinate on the board surface of a piece
	inline uint8_t GetYPos(const piece_data_t piece) {
		return ((0b111 << 1) & piece) >> 1;
	}

	// return true whether piece is unvalid and not ready to be processed
	inline bool CheckUnValidity(const piece_data_t piece) {
		return piece & (0b1 << 7);
	}

	// return true whether given piece data containts same color
	inline bool CheckPDataColor(const piece_data_t piece, const bool col) {
		return (piece & 0b1) == col;
	}

	// set validity tag as unvalid(subordinate piece was propably captured)
	inline void UnSetValidity(piece_data_t& piece) {
		piece |= (0b1 << 7);
	}

	// set validity tag as valid
	inline void SetValidity(piece_data_t& piece) {
		piece &= ~(0b1 << 7);
	}
}

namespace pt = PieceTraits;