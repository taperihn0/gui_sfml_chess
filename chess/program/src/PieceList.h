#pragma once

#include <SFML\Graphics.hpp>

#include <array>
#include <memory>

namespace PieceTraits {

	// piece data for piece list alias
	using piece_data_t = uint8_t;
	
	// PieceList class, container for current positions of entire pieces
	class PieceList {
	public:

		// size of piece list container
		static constexpr uint8_t SIZE = 32;
		using piece_container_t = std::array<piece_data_t, SIZE>;

		PieceList() = default;

		// return piece data based on index from board
		piece_data_t& operator[](const uint8_t index);
		piece_data_t operator[](const uint8_t index) const;

		// Overloaded funtions for changing position of given index piece
		void ChangePos(const uint8_t index, sf::Vector2i new_pos);
		void ChangePos(const uint8_t index, const uint8_t new_x, const uint8_t new_y);

		// begin and end methods returning iterators to container
		// just for convinient of using range-based for loop
		piece_container_t::iterator begin();
		piece_container_t::iterator end();

		piece_container_t::const_iterator begin() const;
		piece_container_t::const_iterator end() const;
	private:
		piece_container_t piece_list;
	};
}
