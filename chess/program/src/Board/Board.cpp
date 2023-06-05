#include "Board.h"

Board::Board(const uint16_t& window_size, const bool& show_console_board_)
	: light_field(sf::Color::Color(196, 164, 132)), 
	dark_field(sf::Color::Color(128, 70, 27)),
	highlighted_field(sf::Color::Color(240, 221, 115, 120)), 
	upgrade_window_color(sf::Color::Color(208, 213, 219)),
	WINDOW_SIZE(window_size), 
	FIELD_SIZE(WINDOW_SIZE / BOARD_SIZE),
	pieces_templates{}, pieces_indicator{}, 
	en_passant_pos(-1, -1), 
	curr_focused_pos(-1, -1), 
	black_king_pos(4, 0),
	white_king_pos(4, BOARD_SIZE - 1), 
	grid_colors{ light_field, dark_field },
	show_console_board(show_console_board_), 
	is_pawn_upgrade_window(false), 
	is_white_turn(true), 
	possible_moves(UINT16_MAX),
	is_turn_sound(false) {

	list_of_window_pieces = {
		PieceFlags::PieceType::QUEEN,
		PieceFlags::PieceType::ROOK,
		PieceFlags::PieceType::KNIGHT,
		PieceFlags::PieceType::BISHOP,
	};
}

sf::SoundSource::Status Board::SoundStatus() {
	return sounds.game_end.getStatus();
}

// arrang starting positions of pieces for both players
void Board::PrepareBoard() {
	render_board.create(WINDOW_SIZE, WINDOW_SIZE);
	plain_board.create(WINDOW_SIZE, WINDOW_SIZE);

	pieces_surface.create(WINDOW_SIZE, WINDOW_SIZE);
	pieces_surface.clear(sf::Color::Color(0, 0, 0, 0));

	PreparePiecesTemplate();
	PrepareAudio();
	InitBoardFields();

	sf::RectangleShape field(sf::Vector2f(FIELD_SIZE, FIELD_SIZE));

	// preparing plain board with fields
	for (uint8_t i = 0; i < BOARD_SIZE; i++) {
		for (uint8_t j = 0; j < BOARD_SIZE; j++) {
			const sf::Vector2f real_window_pos(float(j * FIELD_SIZE), float(i * FIELD_SIZE));

			// setting window position for every field
			// and arranging even-odd fields 
			field.setPosition(real_window_pos);
			field.setFillColor(grid_colors[!((i + j) % 2)]);
			plain_board.draw(field);

			fields_coordinates[i][j] = real_window_pos;

			// drawing default pieces distribution on the transparent
			// pieces surface
			LocatePieceOnSurface(i, j);
		}
	}

	render_board.draw(sf::Sprite(plain_board.getTexture()));
	possible_moves = PreGenerateAllMoves();
}

// draw a piece on his special surface
void Board::DrawOnPiecesSurfaceField(sf::Sprite& piece_sprite, sf::Vector2f& window_pos) {
	piece_sprite.setPosition(window_pos);
	pieces_surface.draw(piece_sprite);
}

// function for drawing board on real game window
sf::Sprite Board::GetBoardSprite() {
	UpdateBoard();
	return sf::Sprite(render_board.getTexture());
}

// size in pixels of a single field on the board
const uint16_t& Board::GetFieldSize() noexcept {
	return FIELD_SIZE;
}

// size of a chess board
constexpr uint8_t Board::GetBoardSize() noexcept {
	return BOARD_SIZE;
}

// return current field, which contain pawn
// which is possible to capture by en passant technique - 
// there is only one such pawn on the board
const sf::Vector2i& Board::GetEnPassantPos() noexcept {
	return en_passant_pos;
}

// process for every single click in the area of game window
void Board::ProcessPressedMouse(const sf::Vector2i& mouse_pos) {
	const sf::Vector2i field_pos((mouse_pos.x - 1) / FIELD_SIZE, (mouse_pos.y - 1) / FIELD_SIZE);

	if (!isValidField(field_pos)) {
		return;
	}

	// variable for moving scenario
	const auto move_field(CheckAndGetIfFocused(field_pos));

	const auto picked_piece(pieces_indicator[field_pos.y][field_pos.x]);
	const bool is_focus_flag(isValidFocused());
	
	// if whites' turn, then focus only white pieces
	// else blacks' turn.
	// but when any piece is already focused,
	// force player to move it or unfocus it.

	if (is_pawn_upgrade_window) {
		PickPieceOnWindow(field_pos);
		return;
	}

	const bool is_turn_color(CheckCurrTurnColor(picked_piece.color));

	// process mouse pressing by focusing a piece,
	// unfocus it or move a piece
	if (!is_focus_flag and picked_piece.type != PieceFlags::PieceType::EMPTY and
		is_turn_color) {
		FocusPieceField(picked_piece, field_pos);
	}
	else if (is_focus_flag and field_pos == curr_focused_pos) {
		UnfocusPieceField(curr_focused_pos);
	}
	else if (is_focus_flag and !move_field.is_found) {
		UnfocusPieceField(curr_focused_pos);
	}
	else if (is_focus_flag and move_field.is_found) {
		MovePiece(move_field.active_clicked);
		UnfocusPieceField(curr_focused_pos);
	}
}

// fill fields with starting postitions of pieces
void Board::InitBoardFields() noexcept {
	const auto&& end_index(BOARD_SIZE - 1);

	// rows of pawns
	pieces_indicator[1].fill
	(PieceFlags::Indicator{ PieceFlags::PieceColor::BLACK, PieceFlags::PieceType::PAWN, 0 });
	pieces_indicator[end_index - 1].fill
	(PieceFlags::Indicator{ PieceFlags::PieceColor::WHITE, PieceFlags::PieceType::PAWN, 0 });

	// other pieces: rooks
	pieces_indicator[0][0] =
		PieceFlags::Indicator{ PieceFlags::PieceColor::BLACK, PieceFlags::PieceType::ROOK, 0 };
	pieces_indicator[0][end_index] =
		PieceFlags::Indicator{ PieceFlags::PieceColor::BLACK, PieceFlags::PieceType::ROOK, 0 };

	pieces_indicator[end_index][0] =
		PieceFlags::Indicator{ PieceFlags::PieceColor::WHITE, PieceFlags::PieceType::ROOK, 0 };
	pieces_indicator[end_index][end_index] =
		PieceFlags::Indicator{ PieceFlags::PieceColor::WHITE, PieceFlags::PieceType::ROOK, 0 };

	// knights
	pieces_indicator[0][1] =
		PieceFlags::Indicator{ PieceFlags::PieceColor::BLACK, PieceFlags::PieceType::KNIGHT, 0 };
	pieces_indicator[0][end_index - 1] =
		PieceFlags::Indicator{ PieceFlags::PieceColor::BLACK, PieceFlags::PieceType::KNIGHT, 0 };

	pieces_indicator[end_index][1] =
		PieceFlags::Indicator{ PieceFlags::PieceColor::WHITE, PieceFlags::PieceType::KNIGHT, 0 };
	pieces_indicator[end_index][end_index - 1] =
		PieceFlags::Indicator{ PieceFlags::PieceColor::WHITE, PieceFlags::PieceType::KNIGHT, 0 };

	// bishops
	pieces_indicator[0][2] =
		PieceFlags::Indicator{ PieceFlags::PieceColor::BLACK, PieceFlags::PieceType::BISHOP, 0 };
	pieces_indicator[0][end_index - 2] =
		PieceFlags::Indicator{ PieceFlags::PieceColor::BLACK, PieceFlags::PieceType::BISHOP, 0 };

	pieces_indicator[end_index][2] =
		PieceFlags::Indicator{ PieceFlags::PieceColor::WHITE, PieceFlags::PieceType::BISHOP, 0 };
	pieces_indicator[end_index][end_index - 2] =
		PieceFlags::Indicator{ PieceFlags::PieceColor::WHITE, PieceFlags::PieceType::BISHOP, 0 };

	// queens
	pieces_indicator[0][3] =
		PieceFlags::Indicator{ PieceFlags::PieceColor::BLACK, PieceFlags::PieceType::QUEEN, 0 };
	pieces_indicator[end_index][3] =
		PieceFlags::Indicator{ PieceFlags::PieceColor::WHITE, PieceFlags::PieceType::QUEEN, 0 };

	// kings
	pieces_indicator[0][4] =
		PieceFlags::Indicator{ PieceFlags::PieceColor::BLACK, PieceFlags::PieceType::KING, 0 };
	pieces_indicator[end_index][4] =
		PieceFlags::Indicator{ PieceFlags::PieceColor::WHITE, PieceFlags::PieceType::KING, 0 };
}


void Board::PreparePiecesTemplate() {
	// pawns templates
	pieces_templates[static_cast<int>(PieceFlags::PieceColor::WHITE)][static_cast<int>(PieceFlags::PieceType::PAWN)]
		= std::make_unique<Pawn>("./program/resource/images/wpawn.png",
			this, FIELD_SIZE, true);
	pieces_templates[static_cast<int>(PieceFlags::PieceColor::BLACK)][static_cast<int>(PieceFlags::PieceType::PAWN)]
		= std::make_unique<Pawn>("./program/resource/images/bpawn.png",
			this, FIELD_SIZE, false);

	// rook templates
	pieces_templates[static_cast<int>(PieceFlags::PieceColor::WHITE)][static_cast<int>(PieceFlags::PieceType::ROOK)]
		= std::make_unique<Rook>("./program/resource/images/wrook.png",
			this, FIELD_SIZE, true);
	pieces_templates[static_cast<int>(PieceFlags::PieceColor::BLACK)][static_cast<int>(PieceFlags::PieceType::ROOK)]
		= std::make_unique<Rook>("./program/resource/images/brook.png",
			this, FIELD_SIZE, false);

	// knight templates
	pieces_templates[static_cast<int>(PieceFlags::PieceColor::WHITE)][static_cast<int>(PieceFlags::PieceType::KNIGHT)]
		= std::make_unique<Knight>("./program/resource/images/wknight.png",
			this, FIELD_SIZE, true);
	pieces_templates[static_cast<int>(PieceFlags::PieceColor::BLACK)][static_cast<int>(PieceFlags::PieceType::KNIGHT)]
		= std::make_unique<Knight>("./program/resource/images/bknight.png",
			this, FIELD_SIZE, false);

	// bishop templates
	pieces_templates[static_cast<int>(PieceFlags::PieceColor::WHITE)][static_cast<int>(PieceFlags::PieceType::BISHOP)]
		= std::make_unique<Bishop>("./program/resource/images/wbishop.png",
			this, FIELD_SIZE, true);
	pieces_templates[static_cast<int>(PieceFlags::PieceColor::BLACK)][static_cast<int>(PieceFlags::PieceType::BISHOP)]
		= std::make_unique<Bishop>("./program/resource/images/bbishop.png",
			this, FIELD_SIZE, false);

	// Queen templates
	pieces_templates[static_cast<int>(PieceFlags::PieceColor::WHITE)][static_cast<int>(PieceFlags::PieceType::QUEEN)]
		= std::make_unique<Queen>("./program/resource/images/wqueen.png",
			this, FIELD_SIZE, true);
	pieces_templates[static_cast<int>(PieceFlags::PieceColor::BLACK)][static_cast<int>(PieceFlags::PieceType::QUEEN)]
		= std::make_unique<Queen>("./program/resource/images/bqueen.png",
			this, FIELD_SIZE, false);

	// and finally king
	pieces_templates[static_cast<int>(PieceFlags::PieceColor::WHITE)][static_cast<int>(PieceFlags::PieceType::KING)]
		= std::make_unique<King>("./program/resource/images/wking.png",
			this, FIELD_SIZE, true);
	pieces_templates[static_cast<int>(PieceFlags::PieceColor::BLACK)][static_cast<int>(PieceFlags::PieceType::KING)]
		= std::make_unique<King>("./program/resource/images/bking.png",
			this, FIELD_SIZE, false);
}

// load from hard drive all the needed sounds
void Board::PrepareAudio() {
	auto load_sbuffer = [](auto sbuffer, const std::string& filepath) {
		if (!sbuffer->loadFromFile(filepath)) {
			exit(3);
		}
	};

	sbuffers[0] = std::make_unique<sf::SoundBuffer>();
	load_sbuffer(sbuffers[0].get(), "./program/resource/audio/capture.ogg");
	sounds.capture.setBuffer(*sbuffers[0]);

	sbuffers[1] = std::make_unique<sf::SoundBuffer>();
	load_sbuffer(sbuffers[1].get(), "./program/resource/audio/castle.ogg");
	sounds.castle.setBuffer(*sbuffers[1]);

	sbuffers[2] = std::make_unique<sf::SoundBuffer>();
	load_sbuffer(sbuffers[2].get(), "./program/resource/audio/game_end.ogg");
	sounds.game_end.setBuffer(*sbuffers[2]);

	sbuffers[3] = std::make_unique<sf::SoundBuffer>();
	load_sbuffer(sbuffers[3].get(), "./program/resource/audio/move_check.ogg");
	sounds.check.setBuffer(*sbuffers[3]);

	sbuffers[4] = std::make_unique<sf::SoundBuffer>();
	load_sbuffer(sbuffers[4].get(), "./program/resource/audio/move_self.ogg");
	sounds.move.setBuffer(*sbuffers[4]);

	sbuffers[5] = std::make_unique<sf::SoundBuffer>();
	load_sbuffer(sbuffers[5].get(), "./program/resource/audio/promote.ogg");
	sounds.promote.setBuffer(*sbuffers[5]);
}

// for every piece in 2d array
// locate it in a proper field in alpha surface
// and generate its occupied fields
void Board::LocatePieceOnSurface(const uint8_t& y, const uint8_t& x) {
	const auto& piece = pieces_indicator[y][x];

	if (piece.type == PieceFlags::PieceType::EMPTY) {
		return;
	}

	pieces_templates[static_cast<int>(piece.color)][static_cast<int>(piece.type)]->
		DrawPiece(fields_coordinates[y][x]);

	SetPieceOccupiedFields(pieces_indicator, piece, y, x);
}


uint16_t Board::PreGenerateAllMoves() {
	uint8_t turn_color(2 - is_white_turn);
	PieceFlags::Indicator piece;
	uint16_t moves_number = 0;

	for (uint8_t i = 0; i < BOARD_SIZE; i++) {
		for (uint8_t j = 0; j < BOARD_SIZE; j++) {
			piece = pieces_indicator[i][j];

			if (piece.type != PieceFlags::PieceType::EMPTY and
				piece.color == static_cast<PieceFlags::PieceColor>(turn_color)) {
				std::vector<sf::Vector2i> avaible_fields = 
					pieces_templates[static_cast<int>(piece.color)][static_cast<int>(piece.type)]->
					GetActiveFields(pieces_indicator, sf::Vector2i(j, i));

				moves_number += static_cast<uint16_t>(avaible_fields.size());
				cache[i][j] = std::move(avaible_fields);
			}
		}
	}

	return moves_number;
}

// focus after clicking on a piece
void Board::FocusPieceField(const PieceFlags::Indicator& picked_piece, const sf::Vector2i& field_pos) {
	
	// drawing highlighted field under the piece
	auto window_field_pos(fields_coordinates[field_pos.y][field_pos.x]);
	sf::RectangleShape field(sf::Vector2f(FIELD_SIZE, FIELD_SIZE));

	field.setPosition(window_field_pos);
	field.setFillColor(highlighted_field);

	plain_board.draw(field);

	// preparing its active fields and then drawing them
	active_focused_field = cache[field_pos.y][field_pos.x];
	
	for (const auto& active_field : active_focused_field) {
		window_field_pos = fields_coordinates[active_field.y][active_field.x];
		field.setPosition(window_field_pos);
		field.setFillColor(highlighted_field);

		plain_board.draw(field);
	}

	// setting current focused piece flag
	curr_focused_pos.y = field_pos.y, curr_focused_pos.x = field_pos.x;
}

// unfocus after clicking exactly on a focused piece
void Board::UnfocusPieceField(const sf::Vector2i& field_pos) {

	// covering highlighted field 
	auto window_field_pos = fields_coordinates[field_pos.y][field_pos.x];
	sf::RectangleShape field(sf::Vector2f(FIELD_SIZE, FIELD_SIZE));

	field.setPosition(window_field_pos);
	field.setFillColor(grid_colors[!((field_pos.y + field_pos.x) % 2)]);

	plain_board.draw(field);

	// covering active fields
	for (const auto& active_field : active_focused_field) {
		window_field_pos = fields_coordinates[active_field.y][active_field.x];
		field.setPosition(window_field_pos);
		field.setFillColor(grid_colors[!((active_field.y + active_field.x) % 2)]);

		plain_board.draw(field);
	}

	// reseting current focused piece flag
	curr_focused_pos.y = -1, curr_focused_pos.x = -1;
}

// move given piece to a given new field - 
// occupy empty field or capture enemy piece there
void Board::MovePiece(const sf::Vector2i& new_move_field) {
	auto moved_piece(pieces_indicator[curr_focused_pos.y][curr_focused_pos.x]),
	    old_piece(pieces_indicator[new_move_field.y][new_move_field.x]);

	// checking for castling action
	if (moved_piece.type == PieceFlags::PieceType::KING and
		old_piece.type == PieceFlags::PieceType::ROOK and
		old_piece.color == moved_piece.color) {
		CastleKingChange(curr_focused_pos, new_move_field);
	}
	else {
		ChangePiecePos(pieces_indicator, curr_focused_pos, new_move_field);
	}

	pieces_indicator[new_move_field.y][new_move_field.x].IncrementMoveCount();
	moved_piece = pieces_indicator[new_move_field.y][new_move_field.x];

	if (moved_piece.type == PieceFlags::PieceType::PAWN) {
		EnPassantCase(new_move_field, moved_piece);
	}

	CheckUpdateIfKingMove(new_move_field);
	ChangePlayersTurn();
	UpdatePiecesSurface();

	// Check if the moved piece was pawn and
	// his new field is in the last row -
	// it means pawn can be upgraded
	
	// Set pawn's first_move flag 
	bool is_upgrade(false);

	if (moved_piece.type == PieceFlags::PieceType::PAWN) {
		is_upgrade = 
			dynamic_cast<Pawn*>(pieces_templates[static_cast<int>(moved_piece.color)][static_cast<int>(moved_piece.type)].get())->
			CheckForUpgrade(pieces_indicator, new_move_field);
	}
	
	if (is_upgrade) {
		OpenPawnUpgradeWindow(new_move_field);
	}

	possible_moves = PreGenerateAllMoves();
}

// zero occuper color of field
void Board::ZeroEntireBoardOccuperColor(std::array<std::array<PieceFlags::Indicator, 8>, 8>& board) {
	for (uint8_t i = 0; i < BOARD_SIZE; i++) {
		for (uint8_t j = 0; j < BOARD_SIZE; j++) {
			board[i][j].occuping_color.white = false;
			board[i][j].occuping_color.black = false;
		}
	}
}

// set all of the piece occupied fields
void Board::SetPieceOccupiedFields(
	std::array<std::array<PieceFlags::Indicator, 8>, 8>& board,
	const PieceFlags::Indicator& piece, const uint8_t& y, const uint8_t& x, bool consider_mate) {
	pieces_templates[static_cast<int>(piece.color)][static_cast<int>(piece.type)]->MarkOccupiedFields(board, sf::Vector2i(x, y), consider_mate);
}

void Board::ChangePiecePos(
	std::array<std::array<PieceFlags::Indicator, 8>, 8>& board,
	sf::Vector2i old_pos, sf::Vector2i new_pos, bool play_sound) noexcept {

	if (play_sound and board[new_pos.y][new_pos.x].type != PieceFlags::PieceType::EMPTY) {
		turn_sound = sounds.capture;
		is_turn_sound = true;
	}
	else if (play_sound) {
		turn_sound = sounds.move;
		is_turn_sound = true;
	}

	// change position
	board[new_pos.y][new_pos.x] =
		board[old_pos.y][old_pos.x];

	// clear his last position
	board[old_pos.y][old_pos.x] =
		PieceFlags::Indicator{ PieceFlags::PieceColor::EMPTY, PieceFlags::PieceType::EMPTY, 0 };
}

// check whether given coordinates are valid for my board
bool Board::isValidField(const sf::Vector2i& coords) noexcept {
	return (coords.x >= 0 and coords.x < BOARD_SIZE) and
		(coords.y >= 0 and coords.y < BOARD_SIZE);
}

// is there a mate on a king of a given color?
bool Board::CheckKingAttacked(
	const std::array<std::array<PieceFlags::Indicator, 8>, 8>& board, 
	PieceFlags::PieceColor king_color) noexcept {
	if (king_color == PieceFlags::PieceColor::WHITE) {
		return board[static_cast<int>(white_king_pos.y)][static_cast<int>(white_king_pos.x)]
			.occuping_color.black;
	}
	return board[static_cast<int>(black_king_pos.y)][static_cast<int>(black_king_pos.x)]
		.occuping_color.white;
}


bool Board::CheckForMateStealMate() {
	if (possible_moves != 0) {
		return false;
	}

	sounds.game_end.play();

	const bool is_king_attacked = is_white_turn?
		CheckKingAttacked(pieces_indicator, PieceFlags::PieceColor::WHITE) : 
		CheckKingAttacked(pieces_indicator, PieceFlags::PieceColor::BLACK);
		
	if (is_white_turn and is_king_attacked) {
		std::cout << "BLACK WON - by checkmate" << '\n';
	}
	else if (!is_white_turn and is_king_attacked) {
		std::cout << "WHITE WON - by checkmate" << '\n';
	}
	else if (!is_king_attacked) {
		std::cout << "TIE - stealmate" << '\n';
	}

	return true;
}

// check whether my focus flag is set
bool Board::isValidFocused() noexcept {
	return curr_focused_pos.x != -1 and curr_focused_pos.y != -1;
}

// check whether field is in focus fields
Board::FieldDataFlag 
Board::CheckAndGetIfFocused(const sf::Vector2i& coords) {
	auto found = std::find(active_focused_field.cbegin(), active_focused_field.cend(), coords);

	if (found != active_focused_field.cend()) {
		return { true, *found };
	}
	return { false, sf::Vector2i() };
}

// check whether given color is the same as current color of moving player
bool Board::CheckCurrTurnColor(const PieceFlags::PieceColor& color) noexcept {
	return color == static_cast<PieceFlags::PieceColor>(static_cast<int>(PieceFlags::PieceColor::BLACK) - is_white_turn);
}

// check if king moved and update its position
void Board::CheckUpdateIfKingMove(sf::Vector2i new_pos) {
	const auto& moved_piece(pieces_indicator[new_pos.y][new_pos.x]);

	if (moved_piece.type != PieceFlags::PieceType::KING) {
		return;
	}

	if (moved_piece.color == PieceFlags::PieceColor::WHITE) {
		white_king_pos = new_pos;
	}
	else {
		black_king_pos = new_pos;
	}
}

// draw window with pieces when pawn is upgrading
void Board::OpenPawnUpgradeWindow(const sf::Vector2i& pos) {
	upgrading_color = pieces_indicator[pos.y][pos.x].color;
	upgrading_x_pos = pos.x;

	uint8_t y = BOARD_SIZE - 1, direct = -1;
	if (upgrading_color == PieceFlags::PieceColor::WHITE) {
		y = 0, direct = 1;
	}
	
	sf::Vector2f window_field_pos;
	sf::RectangleShape field(sf::Vector2f(FIELD_SIZE, FIELD_SIZE));

	field.setFillColor(upgrade_window_color);

	for (const auto& displ_piece : list_of_window_pieces) {
		window_field_pos = fields_coordinates[y][upgrading_x_pos];
		field.setPosition(window_field_pos);
		pieces_surface.draw(field);

		pieces_templates[static_cast<int>(upgrading_color)][static_cast<int>(displ_piece)]->
			DrawPiece(fields_coordinates[y][upgrading_x_pos]);

		y += direct;
	}
	
	turn_sound = sounds.promote;
	is_turn_sound = true;
	is_pawn_upgrade_window = true;
}

// process mouse click - 
// if it's not valid, just wait for 
// proper click pos and leave window turned on
void Board::PickPieceOnWindow(const sf::Vector2i& pos) {
	if (pos.x != upgrading_x_pos) {
		return;
	}
	else if (upgrading_color == PieceFlags::PieceColor::WHITE and 
		pos.y >= 0 and pos.y < list_of_window_pieces.size()) {
		pieces_indicator[0][upgrading_x_pos] = 
			PieceFlags::Indicator{ upgrading_color, list_of_window_pieces[pos.y]};
	}
	else if (upgrading_color == PieceFlags::PieceColor::BLACK and
		pos.y <= BOARD_SIZE - 1 and pos.y > BOARD_SIZE - 1 - list_of_window_pieces.size()) {
		pieces_indicator[BOARD_SIZE - 1][upgrading_x_pos] =
			PieceFlags::Indicator{ upgrading_color, list_of_window_pieces[(BOARD_SIZE - 1) - pos.y] };;
	}

	possible_moves = PreGenerateAllMoves();
	UpdatePiecesSurface();
	is_pawn_upgrade_window = false;
}

// updating render_board sprite
void Board::UpdateBoard() {
	pieces_surface.display();
	plain_board.display();

	// first draw ready plain_board, and then surface of pieces
	render_board.draw(sf::Sprite(plain_board.getTexture()));
	render_board.draw(sf::Sprite(pieces_surface.getTexture()));
	render_board.display();
}

// all the problems with en passant capture in one function -
// capturing and updating current pawn which can be captured using en passant technique
void Board::EnPassantCase(const sf::Vector2i& new_move_field, const PieceFlags::Indicator& moved_piece) {
	const auto& d
		= dynamic_cast<Pawn*>(pieces_templates[static_cast<int>(moved_piece.color)][static_cast<int>(moved_piece.type)].get())->
		GetDirection();

	auto& captured(pieces_indicator[new_move_field.y - d][new_move_field.x]);

	if (captured.type != PieceFlags::PieceType::EMPTY) {
		turn_sound = sounds.capture;
		is_turn_sound = true;
	}

	// capturing pawn using en passant
	if (new_move_field.y - d == en_passant_pos.y) {
		captured =
			PieceFlags::Indicator{ PieceFlags::PieceColor::EMPTY, PieceFlags::PieceType::EMPTY, false };
	}

	// setting en passant position
	const bool is_double_field_move = (moved_piece.color == PieceFlags::PieceColor::WHITE and new_move_field.y == 4) or
		(moved_piece.color == PieceFlags::PieceColor::BLACK and new_move_field.y == 3);

	if (is_double_field_move and moved_piece.CheckMove(1)) {
		SetEnPassantPos(new_move_field.x, new_move_field.y);
	}
	else {
		SetEnPassantPos(-1, -1);
	}
}

// update pieces on their alpha surface
// and set occupied fields of each piece on the surface
void Board::UpdatePiecesSurface() {
	pieces_surface.clear(sf::Color::Color(0, 0, 0, 0));
	ZeroEntireBoardOccuperColor(pieces_indicator);

	for (uint8_t i = 0; i < BOARD_SIZE; i++) {
		for (uint8_t j = 0; j < BOARD_SIZE; j++) {
			LocatePieceOnSurface(i, j);

			if (show_console_board) {
				std::cout << static_cast<int>(pieces_indicator[i][j].type) << ' ';
			}
		}

		if (show_console_board) {
			std::cout << '\n';
		}
	}

	if (!show_console_board) {
		return;
	}

	std::cout << std::string(15, '*') << ' ';

	if (CheckKingAttacked(pieces_indicator, PieceFlags::PieceColor::WHITE)) {
		std::cout << "MATE BLACK";
		turn_sound = sounds.check;

	}
	else if (CheckKingAttacked(pieces_indicator, PieceFlags::PieceColor::BLACK)) {
		std::cout << "MATE WHITE";
		turn_sound = sounds.check;
	}
	
	turn_sound.play();
	is_turn_sound = false;
	std::cout << '\n';
}

// change is_white_turn variable to indicate 
// whose is turn now
void Board::ChangePlayersTurn() noexcept {
	is_white_turn = !is_white_turn;
}


void Board::CastleKingChange(sf::Vector2i old_pos, sf::Vector2i new_pos) {
	ChangePiecePos(pieces_indicator, old_pos, new_pos);
	
	if (new_pos.x == 0) {
		pieces_indicator[new_pos.y][3] =
			PieceFlags::Indicator{ pieces_indicator[new_pos.y][new_pos.x].color, PieceFlags::PieceType::ROOK };
	}
	else {
		pieces_indicator[new_pos.y][5] = 
			PieceFlags::Indicator{ pieces_indicator[new_pos.y][new_pos.x].color, PieceFlags::PieceType::ROOK };
	}

	sounds.castle.play();
}

// reset current en passant position
void Board::SetEnPassantPos(const int& x, const int& y) noexcept {
	en_passant_pos.x = x, en_passant_pos.y = y;
}