#pragma once

#include <SFML\Graphics.hpp>

#include <array>

class Menu {
public:
	Menu(uint16_t window_size_);

	// init function for menu window
	void PrepareMenuWindow();

	// kind of init function for end game window
	void PrepareEndWindow();

	// check mouse position and if the mouse is located in 
	// any button, just widen it
	void ProcessStartMousePos(sf::Event::MouseMoveEvent pos, bool& run_state);

	void ProcessEndMousePos(sf::Event::MouseMoveEvent pos);

	// check every mouse press event while the starting window is opened
	// returns false when need to close the window
	bool ProcessStartMousePress(sf::Vector2i pos, bool& run_state);

	void ProcessEndMousePress(sf::Vector2i pos, bool& run_status);

	void ResetCurrent();

	sf::Sprite GetMenuWindowSprite();
	sf::Sprite GetEndWindowSprite();
private:
	// load resources from hard drive, i.e. font and images
	void LoadResources();

	// prepare displayed text on a end game window
	void PrepareText();

	// init window rectangle texture
	void InitWindowFrame();

	template<typename T>
	void CastGlobalPos(T& pos);

	void WidenText(sf::Text& txt);
	void NarrowText();

	sf::RenderTexture
		end_window_tex,
		menu_window_tex;
	sf::Sprite 
		end_window,
		menu_window;

	sf::RectangleShape frame;

	sf::Texture
		player_icon_tex,
		bot_icon_tex,
		logo_icon_tex;
	sf::Sprite
		player_icon,
		bot_icon,
		logo_icon;

	std::array<sf::Text, 2> menu_text;
	std::array<sf::Text, 5> end_text;
	sf::Text* curr_wide;

	sf::Font italic_f;

	sf::Color 
		menu_color,
		shadow_color;

	sf::FloatRect
		quit_button,
		play_button,
		continue_button;

	const uint16_t WINDOW_SIZE;
	const float 
		X_WINDOW_POS,
		Y_WINDOW_POS;
	static constexpr uint16_t
		WINDOW_WIDTH = 300,
		WINDOW_HEIGHT = 300;
};

