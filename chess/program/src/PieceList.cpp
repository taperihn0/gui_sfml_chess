#include "PieceList.h"

// implementation of PieceList class methods
namespace PieceTraits {
	piece_data_t& PieceList::operator[](const uint8_t index) {
		return piece_list[index];
	}


	piece_data_t PieceList::operator[](const uint8_t index) const {
		return piece_list[index];
	}


	void PieceList::ChangePos(const uint8_t index, sf::Vector2i new_pos) {
		ChangePos(index, new_pos.x, new_pos.y);
	}


	void PieceList::ChangePos(const uint8_t index, const uint8_t new_x, const uint8_t new_y) {
		piece_list[index] = (new_x << 4) | (new_y << 1) | (piece_list[index] & 0b1);
	}


	PieceList::piece_container_t::iterator PieceList::begin() {
		return piece_list.begin();
	}


	PieceList::piece_container_t::iterator PieceList::end() {
		return piece_list.end();
	}


	PieceList::piece_container_t::const_iterator PieceList::begin() const {
		return piece_list.cbegin();
	}


	PieceList::piece_container_t::const_iterator PieceList::end() const {
		return piece_list.cend();
	}
}