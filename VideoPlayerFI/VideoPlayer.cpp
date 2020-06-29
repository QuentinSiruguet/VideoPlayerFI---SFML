#include "VideoPlayer.h"


void VideoPlayer::initVariables()
{
	this->keyPressed = false;
	this->volume = 35;
	this->fullscreenClickTimerMax = 30.f;
	this->fullscreenClickTimer = this->fullscreenClickTimerMax;
	this->fullscreen = false;

	this->mouseVisibleTimerMax = 150.f;
	this->mouseVisibleTimer = 0.f;
	this->hudVisible = true;

	this->soundVisibleTimerMax = 150.f;
	this->soundVisibleTimer = 0.f;
	this->soundVisible = true;
}

void VideoPlayer::initWindow()
{
	this->videoMode = sf::VideoMode(this->mediaSize.x, this->mediaSize.y);
	this->window = new sf::RenderWindow(this->videoMode, this->path, sf::Style::Close);
	this->window->setFramerateLimit(fps);
}

void VideoPlayer::initVLCLIB()
{

	this->instance = new VLC::Instance(0, nullptr);
	this->media = new VLC::Media(*this->instance, this->path, VLC::Media::FromPath);
	this->mp = new VLC::MediaPlayer(*this->media);


	libvlc_media_track_info_t* tracks;

	libvlc_media_parse(this->media->get());
	libvlc_media_get_tracks_info(this->media->get(), &tracks);

	std::vector <VLC::MediaTrack> track = this->media->tracks();

	for (auto& t : track)
	{
		VLC::MediaTrack::Type type = t.type();
		if (type == VLC::MediaTrack::Type::Video)
		{
			this->mediaSize = sf::Vector2f(t.width(), t.height());
			if (sf::VideoMode::getDesktopMode().width * sf::VideoMode::getDesktopMode().height < t.width() * t.height())
			{
				this->mediaSize.x *= sf::VideoMode::getDesktopMode().width / this->mediaSize.x;
				this->mediaSize.y *= sf::VideoMode::getDesktopMode().height / this->mediaSize.y;
			}
		}
	}


	this->e_frame = new sf::Uint8[this->mediaSize.x * this->mediaSize.y * 16];
	libvlc_video_set_callbacks(this->mp->get(), e_lock, unlock, display, this->e_frame);
	this->mp->setVideoFormat("RGBA", this->mediaSize.x, this->mediaSize.y, this->mediaSize.x * 4);

	this->texture.create(this->mediaSize.x, this->mediaSize.y);
	this->sprite.setTexture(this->texture);

	this->mp->play();
	this->mp->setVolume(this->volume);

	this->actualAudioTrack = this->mp->audioTrack();

}

VideoPlayer::VideoPlayer(std::string path, sf::Font* font) : path(path), font(font)
{
	this->initVariables();
	this->initVLCLIB();
	this->initWindow();

	this->HUD = new PlayerHUD(this->window, this->font, this->mp->length());
}

VideoPlayer::~VideoPlayer()
{
	this->mp->stop();
	delete this->instance;
	delete this->media;
	delete this->mp;

	delete this->HUD;

	delete this->e_frame;
	delete this->window;

}

bool VideoPlayer::isRunning()
{
	if (this->window != nullptr)
		return this->window->isOpen();
	else
		return false;
}

void VideoPlayer::pollEvents()
{
	while (this->window->pollEvent(this->SFMLEvent))
	{
		switch (this->SFMLEvent.type)
		{
		case sf::Event::Closed:
			this->window->close();
			break;
			//------------------------------------------------//
			//------------------------------------------------//
		case sf::Event::KeyPressed:
			if (this->SFMLEvent.key.code == sf::Keyboard::Escape)
			{
				if (!this->keyPressed)
				{
					this->keyPressed = true;
					if (!this->fullscreen)
						this->window->close();
					else
						this->toggleFullscreen();
				}
				else
					this->keyPressed = false;
			}
			else if (this->SFMLEvent.key.code == sf::Keyboard::Space)
				this->mp->pause();
			else if (this->SFMLEvent.key.code == sf::Keyboard::V)
			{
				std::cout << "Change Subtitle" << std::endl;
				std::vector <VLC::TrackDescription> desc = this->mp->spuDescription();

				for (int i = 0; i < desc.size(); i++)
					if (desc[i].id() == this->mp->spu())
						this->actualSpuTrack = i;

				if (this->mp->spuCount() > 0)
				{
					if (this->actualSpuTrack + 1 < this->mp->spuCount())
						this->mp->setSpu(desc[this->actualSpuTrack + 1].id());
					else
						this->mp->setSpu(desc[0].id());
				}
			}

			else if (this->SFMLEvent.key.code == sf::Keyboard::B)
			{
				std::cout << "Change Audio" << std::endl;
				std::vector <VLC::TrackDescription> desc = this->mp->audioTrackDescription();

				for (int i = 0; i < desc.size(); i++)
					if (desc[i].id() == this->mp->audioTrack())
						this->actualAudioTrack = i;

				if (this->actualAudioTrack + 1 < this->mp->audioTrackCount())
					this->mp->setAudioTrack(desc[this->actualAudioTrack + 1].id());
				else
					this->mp->setAudioTrack(desc[1].id());
			}
			break;

		case sf::Event::MouseWheelScrolled:
				if (this->SFMLEvent.mouseWheelScroll.delta > 0)
				{
					this->volume += 5;
					if (this->volume > 100)
						this->volume = 100;
					this->mp->setVolume(this->volume);
				}
				else
				{
					this->volume -= 5;
					if (this->volume < 0)
						this->volume = 0;
					this->mp->setVolume(this->volume);
				}
				this->soundVisibleTimer = 0.f;
			break;
		case sf::Event::MouseMoved:
				this->mouseVisibleTimer = 0.f;
				this->hudVisible = true;
				break;
			//------------------------------------------------//
			//------------------------------------------------//
		default:
			break;
		}
	}
}

void VideoPlayer::updateVolumePercent()
{
	if (this->soundVisibleTimer < this->soundVisibleTimerMax)
	{
		this->soundVisibleTimer++;
		this->soundVisible = true;
	}
	else
	{
		this->soundVisible = false;
	}
}

void VideoPlayer::toggleFullscreen()
{
	this->fullscreen = !this->fullscreen;
	if (this->window->hasFocus())
	{
		if (this->fullscreen)
		{
			this->window->create(sf::VideoMode::getDesktopMode(), "", sf::Style::Fullscreen);
			this->window->setFramerateLimit(fps);
			this->sprite.setScale((float)this->window->getSize().x / this->mediaSize.x, (int)(this->window->getSize().y / this->mediaSize.y));
			this->sprite.setPosition(0, (this->window->getSize().y - (int)(this->window->getSize().y / this->mediaSize.y) * this->mediaSize.y) / 2);
		}
		else if (!this->fullscreen)
		{
			this->window->create(this->videoMode, this->path, sf::Style::Close);
			this->window->setFramerateLimit(fps);
			this->sprite.setScale(1, 1);
			this->sprite.setPosition(0, 0);
		}
		this->HUD->toggleFullscreen(this->window);
	}
}

void VideoPlayer::updateMouseVisible()
{
	if(this->mouseVisibleTimer < this->mouseVisibleTimerMax)
		this->mouseVisibleTimer++;

	if (this->hudVisible)
		if (this->mouseVisibleTimer >= this->mouseVisibleTimerMax)
			this->hudVisible = false;


	this->window->setMouseCursorVisible(this->hudVisible);
}

void VideoPlayer::updateInputFullscreen()
{
	if (this->fullscreenClickTimer < this->fullscreenClickTimerMax)
		this->fullscreenClickTimer++;
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
	{
		if (!this->mousePressed)
		{
			if (!this->HUD->getUnclickableGlobalBounds().contains(sf::Mouse::getPosition(*this->window).x, sf::Mouse::getPosition(*this->window).y))
			{
				this->mousePressed = true;
				if (this->fullscreenClickTimer >= this->fullscreenClickTimerMax)
					this->fullscreenClickTimer = 0.f;
				else
					this->toggleFullscreen();
			}
		}
	}
	else
		this->mousePressed = false;
}

void VideoPlayer::update()
{
	this->pollEvents();

	this->updateInputFullscreen();


	this->texture.update(this->e_frame);

	this->updateVolumePercent();
	this->updateMouseVisible();

	this->HUD->update(this->window, this->mp, this->mp->time(), this->volume);

	if (this->mp->time() + 1000 >= this->media->duration())
		this->window->close();
}

void VideoPlayer::render()
{
	this->window->clear();

	this->window->draw(this->sprite);
	this->HUD->render(this->window, this->hudVisible, this->soundVisible);

	this->window->display();

}
