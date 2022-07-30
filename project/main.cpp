#include <TGUI/TGUI.hpp>
#include <SFML/Graphics.hpp>
#include "WorldGenGUI.h"

int main()
{
	sf::RenderWindow window(sf::VideoMode(736u, 368u), "World", sf::Style::Close); //The generated world is to fit inside a 368x184 sized space
	window.setKeyRepeatEnabled(false);
	window.setFramerateLimit(60u);

	WorldGenGUI wgui(&window);

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			wgui.handleEvents(event);
			
			if (event.type == sf::Event::Closed)
				window.close();
		}

		wgui.update();

		window.clear();
		wgui.draw();
		window.display();
	}

	return 0;
}