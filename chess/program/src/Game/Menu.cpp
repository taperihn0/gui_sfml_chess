#include "Menu.h"

Menu::Menu(uint16_t window_size_)
	: curr_wide(nullptr),
	WINDOW_SIZE(window_size_),
	X_WINDOW_POS(static_cast<float>(WINDOW_SIZE) / 2 - WINDOW_WIDTH / 2),
	Y_WINDOW_POS(static_cast<float>(WINDOW_SIZE) / 2 - WINDOW_HEIGHT / 2),
	menu_color(sf::Color::Color(37, 38, 36, 235)),
	shadow_color(sf::Color::Color(30, 31, 29, 245)) {
	LoadResources();
	PrepareText();
	InitWindowFrame();
}


void Menu::PrepareMenuWindow() {
	menu_window_tex.create(WINDOW_WIDTH, WINDOW_HEIGHT);

	logo_icon.setTexture(logo_icon_tex);
	logo_icon.setPosition(WINDOW_WIDTH / 2 - 75, 13);

	// scalling crown logo
	auto curr_size(logo_icon.getTextureRect());
	logo_icon.setScale(150.f / curr_size.width, 150.f / curr_size.height);

	menu_window.setPosition(X_WINDOW_POS, Y_WINDOW_POS);
}


void Menu::PrepareEndWindow() {
	end_window_tex.create(WINDOW_WIDTH, WINDOW_HEIGHT);

	player_icon.setTexture(player_icon_tex);
	bot_icon.setTexture(bot_icon_tex);
	player_icon.setPosition(30, 110);
	bot_icon.setPosition(WINDOW_WIDTH - 115, 110);

	// scalling avatars
	auto curr_size(player_icon.getTextureRect());
	player_icon.setScale(80.f / curr_size.width, 80.f / curr_size.height);

	curr_size = bot_icon.getTextureRect();
	bot_icon.setScale(80.f / curr_size.width, 80.f / curr_size.height);

	// setting end game window position on the window
	end_window.setPosition(X_WINDOW_POS, Y_WINDOW_POS);
}


void Menu::ProcessStartMousePos(sf::Event::MouseMoveEvent pos, bool& run_state) {
	CastGlobalPos(pos);

	bool f1, f2;

	if ((f1 = pos.x > play_button.left and pos.x < play_button.left + play_button.width and
		pos.y > play_button.top and pos.y < play_button.top + play_button.height) and
		curr_wide != &menu_text[0]) {
		WidenText(menu_text[0]);
	}
	else if ((f2 = pos.x > quit_button.left and pos.x < quit_button.left + quit_button.width and
		pos.y > quit_button.top and pos.y < quit_button.top + quit_button.height) and
		curr_wide != &menu_text[1]) {
		WidenText(menu_text[1]);
	}
	else if (!f1 and !f2 and curr_wide != nullptr) {
		NarrowText();
	}
}


void Menu::ProcessEndMousePos(sf::Event::MouseMoveEvent pos) {
	CastGlobalPos(pos);

	bool f1;

	if ((f1 = continue_button.left and pos.x < continue_button.left + continue_button.width and
		pos.y > continue_button.top and pos.y < continue_button.top + continue_button.height) and
		curr_wide != &end_text[4]) {
		WidenText(end_text[4]);
	}
	else if (!f1 and curr_wide != nullptr) {
		NarrowText();
	}
}


void Menu::ResetCurrent() {
	curr_wide = nullptr;
}


bool Menu::ProcessStartMousePress(sf::Vector2i pos, bool& run_state) {
	CastGlobalPos(pos);

	if (pos.x > play_button.left and pos.x < play_button.left + play_button.width and
		pos.y > play_button.top and pos.y < play_button.top + play_button.height) {
		run_state = false;
	}
	else if (pos.x > quit_button.left and pos.x < quit_button.left + quit_button.width and
		pos.y > quit_button.top and pos.y < quit_button.top + quit_button.height) {
		return false;
	}

	return true;
}


void Menu::ProcessEndMousePress(sf::Vector2i pos, bool& run_status) {
	CastGlobalPos(pos);

	if (pos.x > continue_button.left and pos.x < continue_button.left + continue_button.width and
		pos.y > continue_button.top and pos.y < continue_button.top + continue_button.height) {
		run_status = false;
	}
}


sf::Sprite Menu::GetMenuWindowSprite() {
	menu_window_tex.clear(sf::Color::Color(0, 0, 0, 0));

	menu_window_tex.draw(frame);
	menu_window_tex.draw(logo_icon);

	menu_window_tex.draw(menu_text[0]);
	menu_window_tex.draw(menu_text[1]);

	menu_window_tex.display();

	menu_window.setTexture(menu_window_tex.getTexture());
	return menu_window;
}


sf::Sprite Menu::GetEndWindowSprite() {
	end_window_tex.clear(sf::Color::Color(0, 0, 0, 0));

	end_window_tex.draw(frame);
	end_window_tex.draw(bot_icon);
	end_window_tex.draw(player_icon);

	for (const auto& text : end_text) {
		end_window_tex.draw(text);
	}

	end_window_tex.display();

	end_window.setTexture(end_window_tex.getTexture());
	return end_window;
}


void Menu::LoadResources() {
	if (!player_icon_tex.loadFromFile("./program/resource/images/player_icon.png")) {
		exit(3);
	}

	player_icon_tex.setSmooth(true);

	if (!bot_icon_tex.loadFromFile("./program/resource/images/bot_icon.png")) {
		exit(3);
	}

	bot_icon_tex.setSmooth(true);

	if (!logo_icon_tex.loadFromFile("./program/resource/images/crown_icon.png")) {
		exit(3);
	}

	logo_icon_tex.setSmooth(true);

	if (!italic_f.loadFromFile("./program/resource/images/extrabold_italic.ttf")) {
		exit(3);
	}
}


void Menu::PrepareText() {

	// menu text
	menu_text[0].setFont(italic_f);
	menu_text[0].setString("PLAY CHESS");
	menu_text[0].setFillColor(sf::Color::White);
	menu_text[0].setLetterSpacing(2.5);
	menu_text[0].setOutlineThickness(2);
	menu_text[0].setOutlineColor(sf::Color::Black);

	auto bounds(menu_text[0].getLocalBounds());
	menu_text[0].setPosition((WINDOW_WIDTH - bounds.width) / 2, 180);

	play_button.left = menu_text[0].getPosition().x - 4;
	play_button.top = menu_text[0].getPosition().y;
	play_button.width = bounds.width + 2;
	play_button.height = bounds.height + 10;

	menu_text[1].setFont(italic_f);
	menu_text[1].setString("QUIT GAME");
	menu_text[1].setFillColor(sf::Color::White);
	menu_text[1].setLetterSpacing(2.5f);
	menu_text[1].setOutlineThickness(2);
	menu_text[1].setOutlineColor(sf::Color::Black);

	bounds = menu_text[1].getLocalBounds();
	menu_text[1].setPosition((WINDOW_WIDTH - bounds.width) / 2, 230);

	quit_button.left = menu_text[1].getPosition().x - 4;
	quit_button.top = menu_text[1].getPosition().y;
	quit_button.width = bounds.width + 2;
	quit_button.height = bounds.height + 10;

	// end game text
	end_text[0].setFont(italic_f);
	end_text[0].setString("WHITE WON");
	end_text[0].setFillColor(sf::Color::White);
	end_text[0].setLetterSpacing(1.9f);
	end_text[0].setOutlineThickness(2);
	end_text[0].setOutlineColor(sf::Color::Black);

	bounds = end_text[0].getLocalBounds();
	end_text[0].setPosition((WINDOW_WIDTH - bounds.width) / 2, 15);

	end_text[1].setFont(italic_f);
	end_text[1].setString("- by checkmate");
	end_text[1].setFillColor(sf::Color::White);
	end_text[1].setCharacterSize(15);
	end_text[1].setLetterSpacing(3.5f);
	end_text[1].setOutlineThickness(2);
	end_text[1].setOutlineColor(sf::Color::Black);

	bounds = end_text[1].getLocalBounds();
	end_text[1].setPosition((WINDOW_WIDTH - bounds.width) / 2, 55);

	end_text[2].setFont(italic_f);
	end_text[2].setString("VS");
	end_text[2].setFillColor(sf::Color::White);
	end_text[2].setCharacterSize(15);
	end_text[2].setOutlineThickness(2);
	end_text[2].setOutlineColor(sf::Color::Black);

	bounds = end_text[2].getLocalBounds();
	end_text[2].setPosition(WINDOW_WIDTH / 2 - 10, WINDOW_HEIGHT - 160);

	end_text[3].setFont(italic_f);
	end_text[3].setString("-       -       -");
	end_text[3].setFillColor(sf::Color::White);
	end_text[3].setCharacterSize(15);
	end_text[3].setOutlineThickness(2);
	end_text[3].setOutlineColor(sf::Color::Black);

	bounds = end_text[3].getLocalBounds();
	end_text[3].setPosition((WINDOW_WIDTH - bounds.width) / 2, WINDOW_HEIGHT - 95);

	end_text[4].setFont(italic_f);
	end_text[4].setString("continue");
	end_text[4].setFillColor(sf::Color::White);
	end_text[4].setCharacterSize(18);
	end_text[4].setLetterSpacing(5.f);
	end_text[4].setOutlineThickness(2);
	end_text[4].setOutlineColor(sf::Color::Black);

	bounds = end_text[4].getLocalBounds();
	end_text[4].setPosition((WINDOW_WIDTH - bounds.width) / 2, WINDOW_HEIGHT - 55);

	continue_button.left = end_text[4].getPosition().x - 4;
	continue_button.top = end_text[4].getPosition().y;
	continue_button.width = bounds.width + 2;
	continue_button.height = bounds.height + 10;
}


void Menu::InitWindowFrame() {
	frame.setSize(sf::Vector2f(WINDOW_WIDTH - 5, WINDOW_HEIGHT - 5));
	frame.setOutlineThickness(10);
	frame.setOutlineColor(shadow_color);
	frame.setFillColor(menu_color);
}


template<typename T>
void Menu::CastGlobalPos(T& pos) {
	pos.x -= static_cast<int16_t>(end_window.getPosition().x);
	pos.y -= static_cast<int16_t>(end_window.getPosition().y);
}


void Menu::WidenText(sf::Text& txt) {
	curr_wide = &txt;
	auto curr_size(txt.getCharacterSize());
	txt.setCharacterSize(curr_size + 1);
}


void Menu::NarrowText() {
	auto curr_size(curr_wide->getCharacterSize());
	curr_wide->setCharacterSize(curr_size - 1);
	curr_wide = nullptr;
}