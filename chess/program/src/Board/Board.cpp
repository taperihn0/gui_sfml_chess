#include "Board.h"


#ifdef PR_DEBUG
#define CONSOLE_LOG(x, end) std::cout << x << end;
#else
#define CONSOLE_LOG(x, end) 
#endif


Board::Board(const uint16_t& window_size)
	: light_field(sf::Color::Color(196, 164, 132)),
	dark_field(sf::Color::Color(128, 70, 27)),
	highlighted_field(sf::Color::Color(240, 221, 115, 120)),
	upgrade_window_color(sf::Color::Color(208, 213, 219)),
	WINDOW_SIZE(window_size),
	FIELD_SIZE(WINDOW_SIZE / BOARD_SIZE),
	pieces_templates{}, board{}, piece_list{},
	en_passant_pos(-1, -1),
	curr_focused_pos(-1, -1),
	black_king_pos(4, 0),
	white_king_pos(4, BOARD_SIZE - 1),
	grid_colors{ dark_field, light_field },
	is_pawn_upgrade_window(false),
	upgrading_color(),
	upgrading_x_pos(UINT8_MAX),
	is_white_turn(true),
	possible_moves(0),
	is_turn_sound(false),
	engine(board, piece_list, this, &pieces_templates),
	list_of_window_pieces{
			pt::PieceType::QUEEN,
			pt::PieceType::ROOK,
			pt::PieceType::KNIGHT,
			pt::PieceType::BISHOP,
	} 
{}


void Board::PrepareBoard() {
	render_board.create(WINDOW_SIZE, WINDOW_SIZE);
	plain_board.create(WINDOW_SIZE, WINDOW_SIZE);

	pieces_surface.create(WINDOW_SIZE, WINDOW_SIZE);
	pieces_surface.clear(sf::Color::Color(0, 0, 0, 0));

	PreparePiecesTemplate();
	PrepareAudio();
	InitBoardFields();

	sf::RectangleShape field(sf::Vector2f(FIELD_SIZE, FIELD_SIZE));

	// preparing board
	for (uint8_t i = 0; i < BOARD_SIZE; i++) {
		for (uint8_t j = 0; j < BOARD_SIZE; j++) {
			const sf::Vector2f real_window_pos(
				static_cast<float>(j * FIELD_SIZE), 
				static_cast<float>(i * FIELD_SIZE) );

			// setting window position for every field
			// and arranging even-odd fields 
			field.setPosition(real_window_pos);
			field.setFillColor(grid_colors[!((i + j) % 2)]);
			plain_board.draw(field);

			fields_coordinates[i][j] = real_window_pos;
		}
	}
	
	// drawing pieces distribution on the transparent pieces surface
	for (const auto& piece : piece_list) {
		LocatePieceOnSurface(pt::GetYPos(piece), pt::GetXPos(piece));
	}

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


inline uint16_t Board::GetFieldSize() noexcept {
	return FIELD_SIZE;
}


void Board::ProcessPressedMouse(const sf::Vector2i& mouse_pos) {
	const sf::Vector2i field_pos(
		(mouse_pos.x - 1) / FIELD_SIZE, 
		(mouse_pos.y - 1) / FIELD_SIZE );

	///// TEST //////

	for (int i = 0; i < 4; i++) {
		int depth = 4;
		engine.GenerateBestMove(cache, depth, true, en_passant_pos, white_king_pos, black_king_pos);
	}

	// if whites' turn, then focus only white pieces
	// else blacks' turn.
	// but when any piece is already focused,
	// force player to move it or unfocus it.

	if (is_pawn_upgrade_window) {
		PickPieceOnWindow(field_pos);
		// it should be bot's response here
		return;
	}
	else if (!isValidField(field_pos)) {
		return;
	}

	// variable for moving scenario
	const auto move_field = CheckAndGetIfFocused(field_pos);
	const auto picked_piece = board[field_pos.y][field_pos.x];
	const bool 
		is_focus_flag(isValidFocused()),
		is_turn_color(CheckCurrTurnColor(pt::GetColor(picked_piece)));

	// process mouse pressing by focusing a piece,
	// unfocus it or move a piece
	
	if (!is_focus_flag and !pt::CheckType(picked_piece, pt::BIT_WHITE) and is_turn_color) {
		FocusPieceField(picked_piece, field_pos);
	}
	else if (is_focus_flag and (field_pos == curr_focused_pos or !move_field.is_found)) {
		UnfocusPieceField(curr_focused_pos);
	}
	else if (is_focus_flag and move_field.is_found) {
		MovePiece(curr_focused_pos, move_field.active_clicked);
		UnfocusPieceField(curr_focused_pos);

		if (is_pawn_upgrade_window) {
			return;
		}

		// bot's response
		/*const auto bot_move(engine.GenerateBestMove(cache, 1, false, en_passant_pos, white_king_pos, black_king_pos));
		
		if (bot_move.new_pos.x == -1) {
			possible_moves = 0;
			return;
		}

		MovePiece(bot_move.old_pos, bot_move.new_pos, true);*/
	}
}


void Board::InitBoardFields() noexcept {

	// startpos FEN: "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR"
	// other FENs aren't supported
	const std::string FEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
	auto type_hlp = [](const char _char) {
		pt::field_t res;

		switch (_char) {
		case 'p': {
			res = pt::BIT_PAWN;
			break;
		}
		case 'r': {
			res = pt::BIT_ROOK;
			break;
		}
		case 'n': {
			res = pt::BIT_KNIGHT;
			break;
		}
		case 'b': {
			res = pt::BIT_BISHOP;
			break;
		}
		case 'q': {
			res = pt::BIT_QUEEN;
			break;
		}
		case 'k': {
			res = pt::BIT_KING;
			break;
		} 
		default: 
			res = pt::BIT_EMPTY;
		}

		return res;
	};

	uint8_t x(0), y(0), index(0), piece;

	for (const auto& _char : FEN) {
		if (std::isdigit(_char)) {
			x += _char - '1';
			continue;
		}
		else if (_char == '/') {
			y++;
			x = 0;
			continue;
		}
		else if (std::islower(_char)) {
			board[y][x] = pt::BIT_BLACK;
			piece = 0b1;
		}
		else {
			board[y][x] = pt::BIT_WHITE;
			piece = 0b0;
		}

		piece_list[index] = (x << 4) | (y << 1) | piece;
		board[y][x] |= ((index++) << 7) | type_hlp(std::tolower(_char));
		x++;
	}
}


void Board::PreparePiecesTemplate() {
	// pawns templates
	pieces_templates[static_cast<uint8_t>(pt::PieceColor::WHITE)][static_cast<uint8_t>(pt::PieceType::PAWN)]
		= std::make_unique<Pawn>("./program/resource/images/wpawn.png",
			this, FIELD_SIZE, true);
	pieces_templates[static_cast<uint8_t>(pt::PieceColor::BLACK)][static_cast<uint8_t>(pt::PieceType::PAWN)]
		= std::make_unique<Pawn>("./program/resource/images/bpawn.png",
			this, FIELD_SIZE, false);

	// rook templates
	pieces_templates[static_cast<uint8_t>(pt::PieceColor::WHITE)][static_cast<uint8_t>(pt::PieceType::ROOK)]
		= std::make_unique<Rook>("./program/resource/images/wrook.png",
			this, FIELD_SIZE, true);
	pieces_templates[static_cast<uint8_t>(pt::PieceColor::BLACK)][static_cast<uint8_t>(pt::PieceType::ROOK)]
		= std::make_unique<Rook>("./program/resource/images/brook.png",
			this, FIELD_SIZE, false);

	// knight templates
	pieces_templates[static_cast<uint8_t>(pt::PieceColor::WHITE)][static_cast<uint8_t>(pt::PieceType::KNIGHT)]
		= std::make_unique<Knight>("./program/resource/images/wknight.png",
			this, FIELD_SIZE, true);
	pieces_templates[static_cast<uint8_t>(pt::PieceColor::BLACK)][static_cast<uint8_t>(pt::PieceType::KNIGHT)]
		= std::make_unique<Knight>("./program/resource/images/bknight.png",
			this, FIELD_SIZE, false);

	// bishop templates
	pieces_templates[static_cast<uint8_t>(pt::PieceColor::WHITE)][static_cast<uint8_t>(pt::PieceType::BISHOP)]
		= std::make_unique<Bishop>("./program/resource/images/wbishop.png",
			this, FIELD_SIZE, true);
	pieces_templates[static_cast<uint8_t>(pt::PieceColor::BLACK)][static_cast<uint8_t>(pt::PieceType::BISHOP)]
		= std::make_unique<Bishop>("./program/resource/images/bbishop.png",
			this, FIELD_SIZE, false);

	// Queen templates
	pieces_templates[static_cast<uint8_t>(pt::PieceColor::WHITE)][static_cast<uint8_t>(pt::PieceType::QUEEN)]
		= std::make_unique<Queen>("./program/resource/images/wqueen.png",
			this, FIELD_SIZE, true);
	pieces_templates[static_cast<uint8_t>(pt::PieceColor::BLACK)][static_cast<uint8_t>(pt::PieceType::QUEEN)]
		= std::make_unique<Queen>("./program/resource/images/bqueen.png",
			this, FIELD_SIZE, false);

	// and finally king
	pieces_templates[static_cast<uint8_t>(pt::PieceColor::WHITE)][static_cast<uint8_t>(pt::PieceType::KING)]
		= std::make_unique<King>("./program/resource/images/wking.png",
			this, FIELD_SIZE, true);
	pieces_templates[static_cast<uint8_t>(pt::PieceColor::BLACK)][static_cast<uint8_t>(pt::PieceType::KING)]
		= std::make_unique<King>("./program/resource/images/bking.png",
			this, FIELD_SIZE, false);
}


void Board::PrepareAudio() {
	auto load_sbuffer = [](auto* sbuffer, const std::string& filepath) {
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


void Board::LocatePieceOnSurface(const uint8_t y, const uint8_t x) {
	const auto piece_color = pt::GetColor(board[y][x]);
	const auto piece_type = pt::GetType(board[y][x]);
	const auto index = pt::GetPieceListIndex(board[y][x]);

	pieces_templates[static_cast<uint8_t>(piece_color)][static_cast<uint8_t>(piece_type)]->
		DrawPiece(fields_coordinates[y][x]);

	// generating all the possible moves for player whose turn is next in the queue
	// and setting occupied fields of the player which got the move
	// storaging it all in cache memory

	if (piece_type == pt::PieceType::KING) {
		return;
	} // (*) V
	else if (piece_color == static_cast<pt::PieceColor>(1 - is_white_turn)) {
		cache[index] = 
			pieces_templates[static_cast<uint8_t>(piece_color)][static_cast<uint8_t>(piece_type)]->
			GetActiveFields(
				board, sf::Vector2i(x, y), en_passant_pos, 
				(piece_color == pt::PieceColor::WHITE) ? white_king_pos : black_king_pos, piece_list);

		possible_moves += static_cast<uint16_t>(cache[index].size());
		return;
	} // (*) get vector of field on which the figure can stand ... V

	// else prepare occupied fields
	SetPieceOccupiedFields(board, piece_list, y, x);
}


void Board::FocusPieceField(const pt::field_t& picked_piece, const sf::Vector2i& field_pos) {

	// drawing highlighted field under the piece
	auto window_field_pos(fields_coordinates[field_pos.y][field_pos.x]);
	sf::RectangleShape field(sf::Vector2f(FIELD_SIZE, FIELD_SIZE));

	field.setPosition(window_field_pos);
	field.setFillColor(highlighted_field);

	plain_board.draw(field);

	// preparing its active fields and then drawing them
	const auto index = pt::GetPieceListIndex(picked_piece);
	active_focused_field = cache[index];

	for (const auto& active_field : active_focused_field) {
		window_field_pos = fields_coordinates[active_field.y][active_field.x];
		field.setPosition(window_field_pos);
		field.setFillColor(highlighted_field);

		plain_board.draw(field);
	}

	// setting current focused piece flag
	SetFocusedPos(field_pos);
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
	SetFocusedPos(sf::Vector2i(-1, -1));
}


void Board::MovePiece(const sf::Vector2i old_pos, sf::Vector2i new_pos, const bool bot_turn) {
	auto
		moved_piece(board[old_pos.y][old_pos.x]),
		old_piece(board[new_pos.y][new_pos.x]);

	// checking for castling action
	if (pt::CheckType(moved_piece, pt::BIT_KING) and
		pt::CheckType(old_piece, pt::BIT_ROOK) and pt::CheckColor(old_piece, pt::GetBitColor(moved_piece))) {
		SetCastleSound();
		CastleKingChange(board, old_pos, new_pos);
	}
	else {

		// if there is performed any capture, just delete subordinate piece from the piece list
		if (!pt::CheckType(board[new_pos.y][new_pos.x], pt::BIT_EMPTY)) {
			pt::UnSetValidity(piece_list[pt::GetPieceListIndex(board[new_pos.y][new_pos.x])]);
		}

		SetMoveSound(new_pos);
		ChangePiecePos(board, old_pos, new_pos);

		const auto index = pt::GetPieceListIndex(board[new_pos.y][new_pos.x]);
		piece_list.ChangePos(index, new_pos);
	}
	
	CheckUpdateIfKingMove(board, new_pos, white_king_pos, black_king_pos);
	ChangePlayersTurn();

	moved_piece = board[new_pos.y][new_pos.x];
	
	// Check if the moved piece was pawn - this case is a bit tricky
	if (pt::CheckType(moved_piece, pt::BIT_PAWN)) {
		auto* const pawn_t_ptr =
			dynamic_cast<Pawn*>
			(pieces_templates[static_cast<uint8_t>(pt::GetColor(moved_piece))][static_cast<uint8_t>(pt::GetType(moved_piece))].get());

		const auto d = pawn_t_ptr->GetDirection();

		SetEnPassantSound(sf::Vector2i(new_pos.x, new_pos.y - d));
		SetEnPassantPos(EnPassantCase(board, new_pos, d, en_passant_pos.y));

		// check whether its new field is in the last row -
		// it means pawn can be upgraded

		const bool promote_check = pawn_t_ptr->CheckForUpgrade(board, new_pos);

		/*if (promote_check and bot_turn) {
			board[new_pos.y][new_pos.x] =
				pt::CastToBitColor(static_cast<pt::PieceColor>(1 - !is_white_turn));
					//| (engine.GetPromotePiece() ? pt::BIT_QUEEN : pt::BIT_KNIGHT)
		}*/

		UpdatePiecesSurface();

		//if (promote_check and !bot_turn) {
		if (promote_check) {
			OpenPawnUpgradeWindow(new_pos);
		}
	}
	// moving a heavy piece resets current en passant pos
	else {
		SetEnPassantPos(sf::Vector2i(-1, -1));
		pt::SetMoveTag(board[new_pos.y][new_pos.x]);
		UpdatePiecesSurface();
	}
}


void Board::ZeroEntireBoardOccuperColor(pt::board_grid_t& gboard) {
	for (uint8_t i = 0; i < BOARD_SIZE; i++) {
		for (uint8_t j = 0; j < BOARD_SIZE; j++) {
			pt::ZeroOccupers(gboard[i][j]);
		}
	}
}


void Board::SetPieceOccupiedFields(
	pt::board_grid_t& gboard, const pt::PieceList& gpiece_list, uint8_t y, uint8_t x) {
	pieces_templates[static_cast<uint8_t>(pt::GetColor(gboard[y][x]))][static_cast<uint8_t>(pt::GetType(gboard[y][x]))]->
		MarkOccupiedFields(gboard, sf::Vector2i(-1, -1), sf::Vector2i(-1, -1), gpiece_list, sf::Vector2i(x, y));
}


void Board::ChangePiecePos(
	pt::board_grid_t& gboard,
	const sf::Vector2i old_pos, const sf::Vector2i new_pos) noexcept {

	// change position
	gboard[new_pos.y][new_pos.x] =
		gboard[old_pos.y][old_pos.x];

	// clear his last position, but leave his index
	gboard[old_pos.y][old_pos.x] &= (0b11111 << 7);
}


sf::Vector2i Board::EnPassantCase(
	pt::board_grid_t& gboard, const sf::Vector2i new_pos, const int8_t d, const int8_t en_passant_y) {
	auto& moved_piece = gboard[new_pos.y][new_pos.x];

	// capturing pawn using en passant
	if (new_pos.y - d == en_passant_y and new_pos.x == en_passant_pos.x) {
		piece_list[pt::GetPieceListIndex(gboard[new_pos.y - d][new_pos.x])] |= (0b1 << 7);
		gboard[new_pos.y - d][new_pos.x] &= (0b11111 << 7);
	}
	else { // setting en passant position
		
		const bool is_double_field_move =
			(pt::CheckColor(moved_piece, pt::BIT_WHITE) and new_pos.y == 4) or
			(pt::CheckColor(moved_piece, pt::BIT_BLACK) and new_pos.y == 3);

		if (is_double_field_move and !pt::CheckMoveTag(moved_piece)) {
			return new_pos;
		}
	}

	pt::SetMoveTag(moved_piece);
	return { -1, -1 };
}


void Board::CheckUpdateIfKingMove(
	const pt::board_grid_t& gboard, const sf::Vector2i new_pos,
	sf::Vector2i& white_king, sf::Vector2i& black_king) {
	const auto moved_piece = gboard[new_pos.y][new_pos.x];

	if (!pt::CheckType(moved_piece, pt::BIT_KING)) {
		return;
	}

	(pt::CheckColor(moved_piece, pt::BIT_WHITE) ? white_king : black_king) = new_pos;
}


void Board::CastleKingChange(
	pt::board_grid_t& gboard,
	const sf::Vector2i old_pos, sf::Vector2i& new_pos) {

	if (new_pos.x == 0) {
		const auto index = pt::GetPieceListIndex(gboard[new_pos.y][new_pos.x]);
		piece_list.ChangePos(index, 3, new_pos.y);
	
		gboard[new_pos.y][new_pos.x] = 0;

		new_pos = sf::Vector2i(1, new_pos.y);
		gboard[new_pos.y][new_pos.x] = gboard[old_pos.y][old_pos.x];

		gboard[new_pos.y][3] =
			pt::GetBitColor(gboard[new_pos.y][new_pos.x]) | pt::BIT_ROOK;
	}
	else {
		const auto index = pt::GetPieceListIndex(gboard[new_pos.y][new_pos.x]);
		piece_list.ChangePos(index, 5, new_pos.y);

		gboard[new_pos.y][new_pos.x] = 0;

		new_pos = sf::Vector2i(BOARD_SIZE - 2, new_pos.y);
		gboard[new_pos.y][new_pos.x] = gboard[old_pos.y][old_pos.x];

		gboard[new_pos.y][5] =
			pt::GetBitColor(gboard[new_pos.y][new_pos.x]) | pt::BIT_ROOK;
	}

	ChangePiecePos(gboard, old_pos, new_pos);
	pt::SetMoveTag(gboard[new_pos.y][new_pos.x]);

	const auto index = pt::GetPieceListIndex(gboard[new_pos.y][new_pos.x]);
	piece_list.ChangePos(index, new_pos);
}


bool Board::isValidField(sf::Vector2i pos) noexcept {
	return 
		(pos.x >= 0 and pos.x < BOARD_SIZE) and
		(pos.y >= 0 and pos.y < BOARD_SIZE);
}


inline bool Board::CheckKingAttacked(
	const pt::board_grid_t& gboard,
	const pt::PieceColor king_color, const sf::Vector2i check_pos) noexcept {
	return (king_color == pt::PieceColor::WHITE) ?
		pt::CheckOccuper(gboard[check_pos.y][check_pos.x], pt::BIT_OCC_BLACK) :
		pt::CheckOccuper(gboard[check_pos.y][check_pos.x], pt::BIT_OCC_WHITE);
}


bool Board::CheckForMateStealMate() {
	if (possible_moves != 0) {
		return false;
	}

	sounds.game_end.play();

	const bool is_king_attacked = is_white_turn ?
		CheckKingAttacked(board, pt::PieceColor::WHITE, white_king_pos) :
		CheckKingAttacked(board, pt::PieceColor::BLACK, black_king_pos);

	if (is_white_turn and is_king_attacked) {
		CONSOLE_LOG("BLACK WON - by checkmate", '\n');
	}
	else if (!is_white_turn and is_king_attacked) {
		CONSOLE_LOG("WHITE WON - by checkmate", '\n');
	}
	else if (!is_king_attacked) {
		CONSOLE_LOG("TIE - stealmate", '\n');
	}

	return true;
}


inline bool Board::isValidFocused() noexcept {
	return curr_focused_pos.x != -1;
}


Board::FieldDataFlag
Board::CheckAndGetIfFocused(sf::Vector2i pos) {
	auto found = std::find(active_focused_field.cbegin(), active_focused_field.cend(), pos);

	return found != active_focused_field.cend() ? 
		FieldDataFlag{ true, *found } : FieldDataFlag{ false, sf::Vector2i() };
}


inline bool Board::CheckCurrTurnColor(pt::PieceColor color) noexcept {
	return color == 
		static_cast<pt::PieceColor>(static_cast<uint8_t>(pt::PieceColor::BLACK) - is_white_turn);
}


void Board::OpenPawnUpgradeWindow(sf::Vector2i pos) {
	upgrading_color = pt::GetColor(board[pos.y][pos.x]);
	upgrading_x_pos = pos.x;

	uint8_t y(BOARD_SIZE - 1), direct(-1);

	if (upgrading_color == pt::PieceColor::WHITE) {
		y = 0, direct = 1;
	}

	sf::Vector2f window_field_pos;
	sf::RectangleShape field(sf::Vector2f(FIELD_SIZE, FIELD_SIZE));

	field.setFillColor(upgrade_window_color);

	for (const auto& displ_piece : list_of_window_pieces) {
		window_field_pos = fields_coordinates[y][upgrading_x_pos];
		field.setPosition(window_field_pos);
		pieces_surface.draw(field);

		pieces_templates[static_cast<uint8_t>(upgrading_color)][static_cast<uint8_t>(displ_piece)]->
			DrawPiece(fields_coordinates[y][upgrading_x_pos]);

		y += direct;
	}

	turn_sound = sounds.promote;
	is_turn_sound = true;
	is_pawn_upgrade_window = true;
}


void Board::PickPieceOnWindow(sf::Vector2i pos) {
	if (pos.x != upgrading_x_pos) {
		return;
	} // it's new piece here, change it except its piece list index
	else if (upgrading_color == pt::PieceColor::WHITE and pos.y >= 0 and pos.y < list_of_window_pieces.size()) {
		board[0][upgrading_x_pos] =
			(board[0][upgrading_x_pos] & (0b11111 << 7)) |
			pt::CastToBitColor(upgrading_color) | pt::CastToBitType(list_of_window_pieces[pos.y] );
	} // same as above
	else if (upgrading_color == pt::PieceColor::BLACK and 
		pos.y <= BOARD_SIZE - 1 and pos.y > BOARD_SIZE - 1 - list_of_window_pieces.size()) {
		board[BOARD_SIZE - 1][upgrading_x_pos] =
			(board[BOARD_SIZE - 1][upgrading_x_pos] & (0b11111 << 7)) |
			pt::CastToBitColor(upgrading_color) | pt::CastToBitType(list_of_window_pieces[(BOARD_SIZE - 1) - pos.y]);
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
	ZeroEntireBoardOccuperColor(board);
	possible_moves = 0;

	for (const auto& piece : piece_list) {
		if (pt::CheckUnValidity(piece)) {
			continue;
		}

		LocatePieceOnSurface(pt::GetYPos(piece), pt::GetXPos(piece));
	}

	// check king possibilities since the board is fully filled with occupers
	if (is_white_turn) {
		const auto index = pt::GetPieceListIndex(board[white_king_pos.y][white_king_pos.x]);

		cache[index] =
			pieces_templates[static_cast<uint8_t>(pt::PieceColor::WHITE)][static_cast<uint8_t>(pt::PieceType::KING)]->
			GetActiveFields(board, white_king_pos, en_passant_pos, white_king_pos, piece_list);

		possible_moves += static_cast<uint16_t>(cache[index].size());
	}
	else {
		const auto index = pt::GetPieceListIndex(board[black_king_pos.y][black_king_pos.x]);

		cache[index] =
			pieces_templates[static_cast<uint8_t>(pt::PieceColor::BLACK)][static_cast<uint8_t>(pt::PieceType::KING)]->
			GetActiveFields(board, black_king_pos, en_passant_pos, black_king_pos, piece_list);

		possible_moves += static_cast<uint16_t>(cache[index].size());
	}

	if (CheckKingAttacked(board, pt::PieceColor::WHITE, white_king_pos)) {
		CONSOLE_LOG("CHECK BLACK", '\n')
		turn_sound = sounds.check;

	}
	else if (CheckKingAttacked(board, pt::PieceColor::BLACK, black_king_pos)) {
		CONSOLE_LOG("CHECK WHITE", '\n')
		turn_sound = sounds.check;
	}

	turn_sound.play();
	is_turn_sound = false;
}


inline void Board::ChangePlayersTurn() noexcept {
	is_white_turn = !is_white_turn;
}


inline void Board::SetEnPassantPos(const sf::Vector2i new_pos) noexcept {
	en_passant_pos = new_pos;
}


inline void Board::SetFocusedPos(const sf::Vector2i new_pos) noexcept {
	curr_focused_pos = new_pos;
}


void Board::SetMoveSound(sf::Vector2i new_pos) noexcept {
	if (!pt::CheckType(board[new_pos.y][new_pos.x], pt::BIT_EMPTY)) {
		turn_sound = sounds.capture;
		is_turn_sound = true;
	}
	else {
		turn_sound = sounds.move;
		is_turn_sound = true;
	}
}


inline void Board::SetCastleSound() noexcept {
	turn_sound = sounds.castle;
	is_turn_sound = true;
}


void Board::SetEnPassantSound(const sf::Vector2i pos) noexcept {
	if (pt::CheckType(board[pos.y][pos.x], pt::BIT_PAWN)) {
		turn_sound = sounds.capture;
		is_turn_sound = true;
	}
}