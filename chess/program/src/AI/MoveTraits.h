#pragma once

#include "..\PieceData.h"

#include <stack>

class Board;

namespace AI {
	struct piece_pos_change {
		piece_pos_change() = default;
		piece_pos_change(sf::Vector2i old_pos_, sf::Vector2i new_pos_)
			: old_pos(old_pos_), new_pos(new_pos_) {}

		sf::Vector2i
			old_pos,
			new_pos;
	};


	class MoveTraits {
	public:
		MoveTraits(Board* brd_ptr, pt::templates_t* p_templates);

		// Make move based on new position and old position
		void MovePiece(
			pt::board_grid_t& gboard, pt::PieceList& gpiece_list, const sf::Vector2i old_pos, sf::Vector2i new_pos);

		// Undo move performed earlier
		void UnMovePiece(
			pt::board_grid_t& gboard, pt::PieceList& gpiece_list, const sf::Vector2i new_pos);

		// Set current en passant pos
		void SetActualEnPassant(const sf::Vector2i new_en_passant) noexcept;

		// Set current king position 
		void SetKingsPos(const sf::Vector2i white_king, const sf::Vector2i black_king);

		bool CheckPromote() noexcept;

		sf::Vector2i GetPrevEnPassant();

		sf::Vector2i GetKingPos(pt::PieceColor king_col);
	private:
		struct pdata_change {
			uint16_t cpy1;
			bool second;
			uint16_t cpy2;
		};

		void UpdateBoard(pt::board_grid_t& gboard, const pt::PieceList& gpiece_list);

		void EnPassantCase(
			pt::board_grid_t& gboard, pt::PieceList& gpiece_list, const sf::Vector2i new_pos, const int8_t d, pdata_change& chng);

		void CastleCase(
			pt::board_grid_t& gboard, pt::PieceList& gpiece_list, const sf::Vector2i old_pos, sf::Vector2i new_pos, pdata_change& chng);

		Board* const brdclass_ptr;
		pt::templates_t* pieces_templates;

		sf::Vector2i
			en_passant_pos,
			white_king_pos,
			black_king_pos;

		pt::board_grid_t old_board;
		std::stack<sf::Vector2i>
			prev_en_passant,
			prev_white_king,
			prev_black_king;

		std::stack<pt::board_grid_t> prev_brd_states;

		bool promote_flag;

		std::stack<pdata_change> prev_plist_changes;
	};
}