#include "PlayerHUD.h"

std::string PlayerHUD::getCurrentTimeString(float current)
{
	std::stringstream ss;

	int seconds = (int)(current / 1000);
	int minutes = seconds / 60;
	int hours = minutes / 60;

	if (hours % 24 < 10)
		ss << "0";
	ss << hours % 24 << ":";
	if (minutes % 60 < 10)
		ss << "0";
	ss << minutes % 60 << ":";
	if (seconds % 60 < 10)
		ss << "0";
	ss << seconds % 60;

	return ss.str();
}

std::string PlayerHUD::getTimeLeftString(float current)
{
	std::stringstream ss;
	current = this->movieSize - current;
	int seconds = (int)(current / 1000);
	int minutes = seconds / 60;
	int hours = minutes / 60;
	ss << "-";
	if (hours % 24 < 10)
		ss << "0";
	ss << hours % 24 << ":";
	if (minutes % 60 < 10)
		ss << "0";
	ss << minutes % 60 << ":";
	if (seconds % 60 < 10)
		ss << "0";
	ss << seconds % 60;

	return ss.str();
}

void PlayerHUD::initHUD(sf::RenderTarget *target)
{
	this->barFrame.setSize(sf::Vector2f(target->getSize().x - target->getSize().x / 3, 10));
	this->barFrame.setPosition((target->getSize().x - this->barFrame.getSize().x) / 2, target->getSize().y - 30);

	this->barFrame.setFillColor(sf::Color::Transparent);
	this->barFrame.setOutlineThickness(2.f);
	this->barFrame.setOutlineColor(sf::Color(255, 255, 255, 100));

	this->bar.setPosition(this->barFrame.getPosition());
	this->bar.setSize(sf::Vector2f(0, this->barFrame.getSize().y));
	this->bar.setFillColor(sf::Color(255, 255, 255, 200));

	this->unclickableShape.setSize(sf::Vector2f(this->barFrame.getSize().x + 200, this->barFrame.getSize().y * 5 - 100));
	this->unclickableShape.setPosition(this->barFrame.getPosition().x - 100, this->barFrame.getPosition().y + 30);
	this->unclickableShape.setFillColor(sf::Color::Transparent);

	this->button.setRadius(10);
	this->button.setOrigin(5, 5);
}

void PlayerHUD::initText()
{
	this->textCurrentTime.setFont(*this->font);
	this->textCurrentTime.setCharacterSize(15);
	this->textCurrentTime.setFillColor(sf::Color::White);
	this->textCurrentTime.setOutlineColor(sf::Color::Black);
	this->textCurrentTime.setOutlineThickness(2);

	this->textTimeLeft.setFont(*this->font);
	this->textTimeLeft.setCharacterSize(15);
	this->textTimeLeft.setFillColor(sf::Color::White);
	this->textTimeLeft.setOutlineColor(sf::Color::Black);
	this->textTimeLeft.setOutlineThickness(2);

	this->volumePercentage.setFont(*this->font);
	this->volumePercentage.setCharacterSize(25);
	this->volumePercentage.setFillColor(sf::Color::White);
	this->volumePercentage.setOutlineColor(sf::Color::Black);
	this->volumePercentage.setOutlineThickness(2);
}

PlayerHUD::PlayerHUD(sf::RenderTarget* target, sf::Font* font, float movieSize) : movieSize(movieSize), font(font)
{
	this->fullscreen = false;
	this->mousePressed = false;

	this->initHUD(target);
	this->initText();

}

void PlayerHUD::toggleFullscreen(sf::RenderTarget* target)
{
	this->barFrame.setSize(sf::Vector2f(target->getSize().x - target->getSize().x / 3, 10));
	this->barFrame.setPosition((target->getSize().x - this->barFrame.getSize().x) / 2, target->getSize().y - 30);

	this->unclickableShape.setSize(sf::Vector2f(this->barFrame.getSize().x + 200, this->barFrame.getSize().y * 5 - 100));
	this->unclickableShape.setPosition(this->barFrame.getPosition().x - 100, this->barFrame.getPosition().y + 30);

	this->bar.setPosition(this->barFrame.getPosition());
}

sf::FloatRect PlayerHUD::getUnclickableGlobalBounds()
{
	return this->unclickableShape.getGlobalBounds();
}

void PlayerHUD::updateText(float current)
{
	this->textCurrentTime.setString(this->getCurrentTimeString(current));
	this->textCurrentTime.setPosition(this->barFrame.getPosition().x - this->textCurrentTime.getGlobalBounds().width - 10, this->barFrame.getPosition().y - 4);

	this->textTimeLeft.setString(this->getTimeLeftString(current));
	this->textTimeLeft.setPosition(this->barFrame.getPosition().x + this->barFrame.getSize().x + 10, this->barFrame.getPosition().y - 4);
}

void PlayerHUD::updateVolume(float volume)
{
	std::stringstream str;
	str << volume << " %";
	this->volumePercentage.setString(str.str());

	this->volumePercentage.setPosition((this->barFrame.getPosition().x+this->barFrame.getSize().x/2), this->barFrame.getPosition().y - 30);
}

void PlayerHUD::updateBar(sf::RenderWindow* window, VLC::MediaPlayer* mp, float current)
{
	if (barFrame.getGlobalBounds().contains(sf::Mouse::getPosition(*window).x, sf::Mouse::getPosition(*window).y) && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
		if (sf::Mouse::getPosition(*window).x >= this->barFrame.getPosition().x && sf::Mouse::getPosition(*window).x <= this->barFrame.getPosition().x + this->barFrame.getSize().x)
			mp->setTime((sf::Mouse::getPosition(*window).x - this->barFrame.getPosition().x) / this->barFrame.getSize().x * this->movieSize);

	this->bar.setSize(sf::Vector2f((current / this->movieSize) * this->barFrame.getSize().x, this->barFrame.getSize().y));
	this->button.setPosition(this->bar.getPosition().x + this->bar.getSize().x, this->barFrame.getPosition().y);
}

void PlayerHUD::update(sf::RenderWindow* window, VLC::MediaPlayer* mp, float current, float volume)
{
	this->updateVolume(volume);
	this->updateBar(window, mp, current);
	this->updateText(current);
}

void PlayerHUD::render(sf::RenderTarget* target, bool hudVisible, bool soundVisible)
{
	if (soundVisible || hudVisible)
	{
	//	target->draw(this->unclickableShape);
		target->draw(this->barFrame);
		target->draw(this->bar);
		target->draw(this->button);
		target->draw(this->textCurrentTime);
		target->draw(this->textTimeLeft);
		target->draw(this->volumePercentage);
	}

}

