#include "King.h"
#include "..\Board\Board.h"

King::King(const std::string& texture_path, Board* board_ptr,
	const uint16_t& size, const bool& is_white_flag)
	: Piece(texture_path, board_ptr, size, PieceFlags::PieceColor(2 - is_white_flag)),
	directions{ { {-1, -1}, {0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0} } }
{}

// return active fields of king - all of the surrounding fields, except those 
// controlled by enemy
std::vector<sf::Vector2i>&& King::GetActiveFields(
	const std::array<std::array<PieceFlags::Indicator, 8>, 8>& pieces_indicator,
	const sf::Vector2i& pos, bool consider_check, const bool& clear) {
	if (clear) {
		avaible_fields.clear();
	}

	is_check = consider_check;

	for (const auto& direct : directions) {
		const auto& new_pos(pos + sf::Vector2i(direct.d_x, direct.d_y));
	
		if (CheckFieldCheckSafeValid(pieces_indicator, pos, new_pos)) {
			avaible_fields.push_back(new_pos);
		}
	}

	CheckAppendCastleMove(pieces_indicator, pos);

	return std::move(avaible_fields);
}

// check whether field is safe for king
bool King::CheckFieldCheckSafeValid(
	const std::array<std::array<PieceFlags::Indicator, 8>, 8>& pieces_indicator,
	sf::Vector2i old_pos, sf::Vector2i new_pos) noexcept {

	// check if field is valid and can be captured
	bool is_valid = Board::isValidField(new_pos) and
		(pieces_indicator[new_pos.y][new_pos.x].type == PieceFlags::PieceType::EMPTY or
			pieces_indicator[new_pos.y][new_pos.x].color != piece_color);

	if (!is_valid) {
		return false;
	}
	else if (!is_check) {
		return true;
	}

	// prepare copy of current board and then generate all the occupied fields by enemy pieces
	std::array<std::array<PieceFlags::Indicator, 8>, 8> pieces_indicator_cpy = pieces_indicator;

	board->ZeroEntireBoardOccuperColor(pieces_indicator_cpy);

	board->ChangePiecePos(pieces_indicator_cpy, old_pos, new_pos);

	for (uint8_t i = 0; i < 8; i++) {
		for (uint8_t j = 0; j < 8; j++) {
			if (pieces_indicator_cpy[i][j].type != PieceFlags::PieceType::EMPTY and
				pieces_indicator_cpy[i][j].color != piece_color) {
				board->SetPieceOccupiedFields(pieces_indicator_cpy, pieces_indicator_cpy[i][j], i, j, false);
			}
		}
	}

	// decide if the king is safe
	const auto& new_field(pieces_indicator_cpy[new_pos.y][new_pos.x]);

	if (piece_color == PieceFlags::PieceColor::WHITE) {
		return !new_field.occuping_color.black;
	}
	return !new_field.occuping_color.white;
}

// check castling scenario
// conditions: 
// * king can't be attacked
// * it's has to be first move of rook and king
// * between these pieces has to be free space
void King::CheckAppendCastleMove(
	const std::array<std::array<PieceFlags::Indicator, 8>, 8>& pieces_indicator,
	sf::Vector2i pos) {
	if (board->CheckKingAttacked(pieces_indicator, piece_color) or 
		!pieces_indicator[pos.y][pos.x].CheckMove(0)) {
		return;
	}

	std::cout << '.' << '\n';

	// check one side
	auto rook_field(pieces_indicator[pos.y][0]);

	if (rook_field.type == PieceFlags::PieceType::ROOK and
		rook_field.color == piece_color and rook_field.CheckMove(0)) {

		for (uint8_t x = pos.x - 1; x > 0; x--) {
			if (pieces_indicator[pos.y][x].type != PieceFlags::PieceType::EMPTY) {
				return;
			}
		}

		avaible_fields.push_back(sf::Vector2i(0, pos.y));
	}

	// check another side
	rook_field = pieces_indicator[pos.y][8 - 1];

	if (rook_field.type == PieceFlags::PieceType::ROOK and
		rook_field.color == piece_color and rook_field.CheckMove(0)) {

		for (uint8_t x = pos.x + 1; x < 8 - 1; x++) {
			if (pieces_indicator[pos.y][x].type != PieceFlags::PieceType::EMPTY) {
				return;
			}
		}

		avaible_fields.push_back(sf::Vector2i(8 - 1, pos.y));
	}
}