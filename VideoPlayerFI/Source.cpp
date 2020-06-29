
#include "VideoPlayer.h"

int main(int argc, char* argv[])
{
	std::string path;
	if (argc < 2)
		return 0;


	sf::Font font;
	std::stringstream fontdir;
	std::stringstream fontdirend;
	fontdir << argv[0];
	for (int i = 0; i < fontdir.str().size()-17; i++)
		fontdirend << fontdir.str()[i];
	fontdirend << "Ressources/Font/LemonMilk.otf";
	font.loadFromFile(fontdirend.str());


	VideoPlayer* VP;
	VP = new VideoPlayer(argv[1], &font);

	while (VP != nullptr)
	{
		if (!VP->isRunning())
		{
			delete VP;
			VP = nullptr;
		}
		else if (VP->isRunning())
		{
			VP->update();
			VP->render();
		}
	}

}