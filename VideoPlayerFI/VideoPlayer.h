#pragma once
#include <thread>
#include <iostream>
#include "PlayerHUD.h"

class VideoPlayer
{
private:
	//Window + VLC Things
	sf::VideoMode videoMode;
	sf::RenderWindow* window;
	sf::Event SFMLEvent;

	sf::Font* font;

	sf::Texture texture;
	sf::Sprite sprite;

	sf::Uint8 *e_frame;

	std::string path;

	VLC::Instance *instance;
	VLC::Media *media;
	VLC::MediaPlayer *mp;

	sf::Vector2f mediaSize;

	PlayerHUD *HUD;

	int actualAudioTrack;
	int actualSpuTrack;
	//Logic
	bool keyPressed;
	bool mousePressed;

	int volume;
	int fps = 60;
	
	float mouseVisibleTimerMax;
	float mouseVisibleTimer;
	bool hudVisible;

	float fullscreenClickTimerMax;
	float fullscreenClickTimer;
	bool fullscreen;


	void initVariables();
	void initWindow();
	void initVLCLIB();

public:
	VideoPlayer(std::string path, sf::Font *font);
	~VideoPlayer();

	bool isRunning();

	void pollEvents();
	void toggleFullscreen();

	void updateMouseVisible();
	void updateInputFullscreen();
	void update();
	void render();
};

struct ctx
{
	sf::Uint8* surf;
};

static void* e_lock(void* data, void** p_pixels)
{
	struct ctx* ctx = (struct ctx*)data;
	*p_pixels = ctx;
	return NULL;
}

static void unlock(void* data, void* id, void* const* p_pixels)
{
	struct ctx* ctx = (struct ctx*)data;

	assert(id == NULL);
}

static void display(void* data, void* id)
{
	(void)data;
	assert(id == NULL);
}