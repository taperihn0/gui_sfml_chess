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

	// check mouse position when the starting window is opened 
	// and if the mouse is located in any button, just widen it
	void ProcessStartMousePos(sf::Event::MouseMoveEvent pos, bool& run_state);

	// same as above method but when the end window is opened
	void ProcessEndMousePos(sf::Event::MouseMoveEvent pos);

	// check every mouse press event while the starting window is opened
	// returns false when need to close the window
	bool ProcessStartMousePress(sf::Vector2i pos, bool& run_state);

	// again same as above but check when the end window is opened
	void ProcessEndMousePress(sf::Vector2i pos, bool& run_status);
	
	// reset current focus button
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

	// cast given mouse pos to a local coordinates system
	template<typename T>
	void CastGlobalPos(T& pos);

	// focus a field when the mouse is currently on button
	void WidenText(sf::Text& txt);

	// unfocus button
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

