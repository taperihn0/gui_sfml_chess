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
	upgrading_color(PieceFlags::PieceColor::EMPTY),
	upgrading_x_pos(UINT8_MAX),
	is_white_turn(true),
	possible_moves(0),
	is_turn_sound(false),
	engine(pieces_indicator, this, &pieces_templates) {

	list_of_window_pieces = {
		PieceFlags::PieceType::QUEEN,
		PieceFlags::PieceType::ROOK,
		PieceFlags::PieceType::KNIGHT,
		PieceFlags::PieceType::BISHOP,
	};
}


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

	is_white_turn = true;
	render_board.draw(sf::Sprite(plain_board.getTexture()));
}


void Board::DrawOnPiecesSurfaceField(sf::Sprite& piece_sprite, sf::Vector2f& window_pos) {
	piece_sprite.setPosition(window_pos);
	pieces_surface.draw(piece_sprite);
}


sf::Sprite Board::GetBoardSprite() {
	UpdateBoard();
	return sf::Sprite(render_board.getTexture());
}


const uint16_t& Board::GetFieldSize() noexcept {
	return FIELD_SIZE;
}


constexpr uint8_t Board::GetBoardSize() noexcept {
	return BOARD_SIZE;
}


const sf::Vector2i& Board::GetEnPassantPos() noexcept {
	return en_passant_pos;
}


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
		MovePiece(curr_focused_pos, move_field.active_clicked);
		UnfocusPieceField(curr_focused_pos);

		// bot's response
		const auto bot_move_board(engine.GenerateBestMove(cache, 3, false, en_passant_pos));
		pieces_indicator = bot_move_board;

		UpdatePiecesSurface();
		is_white_turn = true;
	}
}


void Board::InitBoardFields() noexcept {
	const auto end_index(BOARD_SIZE - 1);

	// empty rows
	for (uint8_t i = 2; i <= 5; i++) {
		pieces_indicator[i] = {};
	}

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


void Board::LocatePieceOnSurface(const uint8_t& y, const uint8_t& x) {
	const auto piece = pieces_indicator[y][x];

	if (piece.type == PieceFlags::PieceType::EMPTY) {
		return;
	}

	pieces_templates[static_cast<int>(piece.color)][static_cast<int>(piece.type)]->
		DrawPiece(fields_coordinates[y][x]);

	if (piece.type == PieceFlags::PieceType::KING) {
		return;
	}

	const auto active_fields(
		pieces_templates[static_cast<int>(piece.color)][static_cast<int>(piece.type)]->GetActiveFields(pieces_indicator, sf::Vector2i(x, y)));

	if (piece.color != static_cast<PieceFlags::PieceColor>(2 - is_white_turn)) {
		for (const auto& field : active_fields) {
			pieces_templates[static_cast<int>(piece.color)][static_cast<int>(piece.type)]->MarkSingleOccupied(pieces_indicator[field.y][field.x]);
		}
	}
	else if (piece.color == static_cast<PieceFlags::PieceColor>(2 - is_white_turn)) {
		cache[y][x] = active_fields;
		possible_moves += static_cast<uint16_t>(cache[y][x].size());
	}
}


/*uint16_t Board::PreGenerateAllMoves() {
	auto turn_col(static_cast<PieceFlags::PieceColor>(2 - is_white_turn));
	PieceFlags::Indicator piece;
	uint16_t moves_number(0);

	for (uint8_t i = 0; i < BOARD_SIZE; i++) {
		for (uint8_t j = 0; j < BOARD_SIZE; j++) {
			piece = pieces_indicator[i][j];

			if (piece.type == PieceFlags::PieceType::EMPTY or turn_col != piece.color) {
				continue;
			}

			std::vector<sf::Vector2i> avaible_fields(
				pieces_templates[static_cast<int>(piece.color)][static_cast<int>(piece.type)]->
				GetActiveFields(pieces_indicator, sf::Vector2i(j, i)));

			moves_number += static_cast<uint16_t>(avaible_fields.size());
			cache[i][j] = std::move(avaible_fields);
		}
	}

	return moves_number;
}*/


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


void Board::MovePiece(const sf::Vector2i old_pos, const sf::Vector2i new_pos) {
	auto moved_piece(pieces_indicator[old_pos.y][old_pos.x]),
		old_piece(pieces_indicator[new_pos.y][new_pos.x]);

	// checking for castling action
	if (moved_piece.type == PieceFlags::PieceType::KING and
		old_piece.type == PieceFlags::PieceType::ROOK and old_piece.color == moved_piece.color) {
		SetCastleSound();
		CastleKingChange(pieces_indicator, old_pos, new_pos);
	}
	else {
		SetMoveSound(new_pos);
		ChangePiecePos(pieces_indicator, old_pos, new_pos);
	}
	
	moved_piece = pieces_indicator[new_pos.y][new_pos.x];

	SetEnPassantPos(sf::Vector2i(-1, -1));
	//en_passant_pos = sf::Vector2i(-1, -1);
	if (moved_piece.type == PieceFlags::PieceType::PAWN) {
		const auto d(
			dynamic_cast<Pawn*>(pieces_templates[static_cast<int>(moved_piece.color)][static_cast<int>(moved_piece.type)].get())->
			GetDirection());

		SetEnPassantSound(sf::Vector2i(new_pos.x, new_pos.y - d));
		auto new_en_passant_pos(EnPassantCase(pieces_indicator, new_pos, moved_piece, d));

		SetEnPassantPos(new_en_passant_pos);
	}

	CheckUpdateIfKingMove(new_pos);
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
			CheckForUpgrade(pieces_indicator, new_pos);
	}

	if (is_upgrade) {
		OpenPawnUpgradeWindow(new_pos);
	}
}


void Board::ZeroEntireBoardOccuperColor(PieceFlags::board_grid_t& board) {
	for (uint8_t i = 0; i < BOARD_SIZE; i++) {
		for (uint8_t j = 0; j < BOARD_SIZE; j++) {
			board[i][j].occuping_color.white = false;
			board[i][j].occuping_color.black = false;
		}
	}
}


void Board::SetPieceOccupiedFields(
	PieceFlags::board_grid_t& board, const uint8_t& y, const uint8_t& x, bool consider_mate) {
	pieces_templates[static_cast<int>(board[y][x].color)][static_cast<int>(board[y][x].type)]->MarkOccupiedFields(board, sf::Vector2i(x, y), consider_mate);
}


void Board::ChangePiecePos(
	PieceFlags::board_grid_t& board,
	sf::Vector2i old_pos, sf::Vector2i new_pos) noexcept {

	// change position
	board[new_pos.y][new_pos.x] =
		board[old_pos.y][old_pos.x];

	// clear his last position
	board[old_pos.y][old_pos.x] =
		PieceFlags::Indicator{ PieceFlags::PieceColor::EMPTY, PieceFlags::PieceType::EMPTY, 0 };

	pieces_indicator[new_pos.y][new_pos.x].move_count++;
}


sf::Vector2i Board::EnPassantCase(
	PieceFlags::board_grid_t& board, const sf::Vector2i new_pos, const PieceFlags::Indicator moved_piece, const int8_t d) {

	// capturing pawn using en passant
	if (new_pos.y - d == en_passant_pos.y) {
		pieces_indicator[new_pos.y - d][new_pos.x] =
			PieceFlags::Indicator{ PieceFlags::PieceColor::EMPTY, PieceFlags::PieceType::EMPTY };
	}

	// setting en passant position
	const bool is_double_field_move(
		(moved_piece.color == PieceFlags::PieceColor::WHITE and new_pos.y == 4) or
		(moved_piece.color == PieceFlags::PieceColor::BLACK and new_pos.y == 3));

	if (is_double_field_move and moved_piece.CheckMove(1)) {
		return new_pos;
	}

	return { -1, -1 };
}


void Board::CastleKingChange(
	PieceFlags::board_grid_t& board,
	sf::Vector2i old_pos, sf::Vector2i new_pos) {
	ChangePiecePos(board, old_pos, new_pos);

	if (new_pos.x == 0) {
		board[new_pos.y][3] =
			PieceFlags::Indicator{ board[new_pos.y][new_pos.x].color, PieceFlags::PieceType::ROOK };
	}
	else {
		board[new_pos.y][5] =
			PieceFlags::Indicator{ board[new_pos.y][new_pos.x].color, PieceFlags::PieceType::ROOK };
	}
}


bool Board::isValidField(const sf::Vector2i& coords) noexcept {
	return (coords.x >= 0 and coords.x < BOARD_SIZE) and
		(coords.y >= 0 and coords.y < BOARD_SIZE);
}


bool Board::CheckKingAttacked(
	const PieceFlags::board_grid_t& board,
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

	const bool is_king_attacked = is_white_turn ?
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


bool Board::isValidFocused() noexcept {
	return curr_focused_pos.x != -1 and curr_focused_pos.y != -1;
}


Board::FieldDataFlag
Board::CheckAndGetIfFocused(const sf::Vector2i& coords) {
	auto found = std::find(active_focused_field.cbegin(), active_focused_field.cend(), coords);

	if (found != active_focused_field.cend()) {
		return { true, *found };
	}
	return { false, sf::Vector2i() };
}


bool Board::CheckCurrTurnColor(const PieceFlags::PieceColor& color) noexcept {
	return color == static_cast<PieceFlags::PieceColor>(static_cast<int>(PieceFlags::PieceColor::BLACK) - is_white_turn);
}


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


void Board::PickPieceOnWindow(const sf::Vector2i& pos) {
	if (pos.x != upgrading_x_pos) {
		return;
	}
	else if (upgrading_color == PieceFlags::PieceColor::WHITE and
		pos.y >= 0 and pos.y < list_of_window_pieces.size()) {
		pieces_indicator[0][upgrading_x_pos] =
			PieceFlags::Indicator{ upgrading_color, list_of_window_pieces[pos.y] };
	}
	else if (upgrading_color == PieceFlags::PieceColor::BLACK and
		pos.y <= BOARD_SIZE - 1 and pos.y > BOARD_SIZE - 1 - list_of_window_pieces.size()) {
		pieces_indicator[BOARD_SIZE - 1][upgrading_x_pos] =
			PieceFlags::Indicator{ upgrading_color, list_of_window_pieces[(BOARD_SIZE - 1) - pos.y] };;
	}

	UpdatePiecesSurface();
	is_pawn_upgrade_window = false;
}


void Board::UpdateBoard() {
	pieces_surface.display();
	plain_board.display();

	// first draw ready plain_board, and then surface of pieces
	render_board.draw(sf::Sprite(plain_board.getTexture()));
	render_board.draw(sf::Sprite(pieces_surface.getTexture()));
	render_board.display();
}


void Board::UpdatePiecesSurface() {
	pieces_surface.clear(sf::Color::Color(0, 0, 0, 0));
	ZeroEntireBoardOccuperColor(pieces_indicator);
	possible_moves = 0;

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

	// check king possibilities since the board is fully filled with occupers
	if (is_white_turn) {
		cache[white_king_pos.y][white_king_pos.x] =
			pieces_templates[static_cast<int>(PieceFlags::PieceColor::WHITE)][static_cast<int>(PieceFlags::PieceType::KING)]->
			GetActiveFields(pieces_indicator, white_king_pos);
	}
	else {
		cache[white_king_pos.y][white_king_pos.x] =
			pieces_templates[static_cast<int>(PieceFlags::PieceColor::BLACK)][static_cast<int>(PieceFlags::PieceType::KING)]->
			GetActiveFields(pieces_indicator, black_king_pos);
	}

	if (show_console_board) {
		std::cout << std::string(15, '*') << '\n';
	}

	if (CheckKingAttacked(pieces_indicator, PieceFlags::PieceColor::WHITE)) {
		std::cout << "CHECK BLACK" << '\n';
		turn_sound = sounds.check;

	}
	else if (CheckKingAttacked(pieces_indicator, PieceFlags::PieceColor::BLACK)) {
		std::cout << "CHECK WHITE" << '\n';
		turn_sound = sounds.check;
	}

	turn_sound.play();
	is_turn_sound = false;
}


void Board::ChangePlayersTurn() noexcept {
	is_white_turn = !is_white_turn;
}


void Board::SetEnPassantPos(const sf::Vector2i new_pos) noexcept {
	en_passant_pos = new_pos;
}


void Board::SetMoveSound(sf::Vector2i new_pos) noexcept {
	if (pieces_indicator[new_pos.y][new_pos.x].type != PieceFlags::PieceType::EMPTY) {
		turn_sound = sounds.capture;
		is_turn_sound = true;
	}
	else {
		turn_sound = sounds.move;
		is_turn_sound = true;
	}
}


void Board::SetCastleSound() noexcept {
	turn_sound = sounds.castle;
	is_turn_sound = true;
}


void Board::SetEnPassantSound(sf::Vector2i pos) noexcept {
	if (pieces_indicator[pos.y][pos.x].type == PieceFlags::PieceType::PAWN) {
		turn_sound = sounds.capture;
		is_turn_sound = true;
	}
}