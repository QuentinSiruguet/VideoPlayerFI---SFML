#pragma once

#include "SFML/Graphics.hpp"
#include "vlc.hpp"
#include <sstream>
#include <ctime>
#include <iostream>

class PlayerHUD
{
private:
	sf::RectangleShape bar;
	sf::RectangleShape barFrame;
	sf::RectangleShape unclickableShape;

	sf::CircleShape button;
	sf::Font* font;
	sf::Text textCurrentTime;
	sf::Text textTimeLeft;
	sf::Text volumePercentage;

	float movieSize;

	bool mousePressed;
	bool fullscreen;

	float volumeTimer;
	float volumeTimerMax;

	std::string getCurrentTimeString(float current);
	std::string getTimeLeftString(float current);

	void initHUD(sf::RenderTarget *target);
	void initText();

public:
	PlayerHUD(sf::RenderTarget* target, sf::Font* font, float movieSize);

	void toggleFullscreen(sf::RenderTarget* target);

	sf::FloatRect getUnclickableGlobalBounds();

	void updateText(float current);
	void updateVolume(float volume);
	void updateBar(sf::RenderWindow* window, VLC::MediaPlayer* mp, float current);
	void update(sf::RenderWindow* window, VLC::MediaPlayer* mp, float current, float volume);

	void render(sf::RenderTarget* target, bool hudVisible, bool soundVisible);
};

