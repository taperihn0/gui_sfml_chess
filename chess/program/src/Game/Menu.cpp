#include "Menu.h"

Menu::Menu(uint16_t window_size_)
	: WINDOW_SIZE(window_size_),
	menu_color(sf::Color::Color(37, 38, 36, 230))
{}


void Menu::PrepareEndWindow() {
	end_window_tex.create(END_WINDOW_WIDTH, END_WINDOW_HEIGHT);
	LoadResources();
	PrepareText();

	player_icon.setTexture(player_icon_tex);
	bot_icon.setTexture(bot_icon_tex);
	player_icon.setPosition(30, 110);
	bot_icon.setPosition(END_WINDOW_WIDTH - 115, 110);

	// scalling avatars
	auto curr_size(player_icon.getTextureRect());
	player_icon.setScale(80.f / curr_size.width, 80.f / curr_size.height);

	curr_size = bot_icon.getTextureRect();
	bot_icon.setScale(80.f / curr_size.width, 80.f / curr_size.height);
	// ... //

	frame.setSize(sf::Vector2f(END_WINDOW_WIDTH - 5, END_WINDOW_HEIGHT - 5));
	frame.setOutlineThickness(10);
	frame.setOutlineColor(menu_color);
	frame.setFillColor(menu_color);

	end_window_tex.draw(frame);
	end_window_tex.draw(bot_icon);
	end_window_tex.draw(player_icon);
	
	end_window_tex.draw(text[0]);
	end_window_tex.draw(text[1]);
	end_window_tex.draw(text[2]);
	end_window_tex.draw(text[3]);
	end_window_tex.draw(text[4]);

	end_window_tex.display();

	// setting end game window position on the window
	end_window.setTexture(end_window_tex.getTexture());
	end_window.setPosition(WINDOW_SIZE / 2 - END_WINDOW_WIDTH / 2, WINDOW_SIZE / 2 - END_WINDOW_HEIGHT / 2);
}


sf::Sprite Menu::GetEndWindowSprite() {
	return end_window;
}


void Menu::LoadResources() {
	if (!player_icon_tex.loadFromFile("./program/resource/images/player_icon.png")) {
		exit(3);
	}

	if (!bot_icon_tex.loadFromFile("./program/resource/images/bot_icon.png")) {
		exit(3);
	}

	if (!italic_f.loadFromFile("./program/resource/images/extrabold_italic.ttf")) {
		exit(3);
	}
}


void Menu::PrepareText() {
	text[0].setFont(italic_f);
	text[0].setString("WHITE WON");
	text[0].setFillColor(sf::Color::White);
	text[0].setLetterSpacing(1.9);

	auto bounds(text[0].getLocalBounds());
	text[0].setPosition((END_WINDOW_WIDTH - bounds.width) / 2, 15);

	text[1].setFont(italic_f);
	text[1].setString("- by checkmate");
	text[1].setFillColor(sf::Color::White);
	text[1].setCharacterSize(15);
	text[1].setLetterSpacing(3.5);

	bounds = text[1].getLocalBounds();
	text[1].setPosition((END_WINDOW_WIDTH - bounds.width) / 2, 55);

	text[2].setFont(italic_f);
	text[2].setString("VS");
	text[2].setFillColor(sf::Color::White);
	text[2].setCharacterSize(15);

	bounds = text[2].getLocalBounds();
	text[2].setPosition(END_WINDOW_WIDTH / 2 - 10, END_WINDOW_HEIGHT - 160);

	text[3].setFont(italic_f);
	text[3].setString("1       :       0");
	text[3].setFillColor(sf::Color::White);
	text[3].setCharacterSize(15);

	bounds = text[3].getLocalBounds();
	text[3].setPosition((END_WINDOW_WIDTH - bounds.width) / 2, END_WINDOW_HEIGHT - 95);

	text[4].setFont(italic_f);
	text[4].setString("continue");
	text[4].setFillColor(sf::Color::White);
	text[4].setCharacterSize(18);
	text[4].setLetterSpacing(5);

	bounds = text[4].getLocalBounds();
	text[4].setPosition((END_WINDOW_WIDTH - bounds.width) / 2, END_WINDOW_HEIGHT - 55);
}