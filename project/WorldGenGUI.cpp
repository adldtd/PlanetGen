#include "WorldGenGUI.h"
#include <TGUI/TGUI.hpp>
#include <SFML/Graphics.hpp>

WorldGenGUI::WorldGenGUI()
{
	progress = 0;
	lastProgress = 0;
	inProgress = false;

	initialize();
}

WorldGenGUI::~WorldGenGUI()
{
	delete[] buffer;
	delete[] elevation;
	delete[] moisture;
	delete[] climate;
}

void WorldGenGUI::initialize()
{

}



void WorldGenGUI::update()
{
	phone.lock(); //Load pixels on tile map
	if (inProgress) {
		while (lastProgress < progress) //******************************************** MAKE SURE LASTPROGRESS OR PROGRESS DO NOT OVERFLOW
		{
			lastProgress++;
			int index = lastProgress % length;
			unsigned int tileX = index % lengthX; unsigned int tileY = index / lengthX;

			int coloring[4] = { 0, 0, 0, 255 };
			returnRGBA(buffer[index], coloring);
			map.updateTile(sf::Vector2u(tileX, tileY), coloring[0], coloring[1], coloring[2], coloring[3]);
		}

		lastProgress--;
	}
	phone.unlock();
}

void WorldGenGUI::fitToSpace(sf::Vector2f coordinates, sf::Vector2f lengths)
{
	int w = map.getWidth(); //Each is measured in pixels
	int h = map.getHeight();

	float idealFactor = lengths.x / (float)lengths.y;
	float tileFactor = w / (float)h;

	if (tileFactor == idealFactor) //Fits perfectly
	{
		float scale = lengths.x / (float)w;
		map.loadMap(sf::Vector2u(1u, 1u), scale);
		map.setPosition(coordinates);
	}
	else if (tileFactor < idealFactor) //Width is too small; fit to height
	{
		float scale = lengths.y / (float)h;
		float offset = ((float)lengths.x - (w * scale)) / 2.0; //Center by width
		map.loadMap(sf::Vector2u(1u, 1u), scale);
		map.setPosition(sf::Vector2f(coordinates.x + offset, coordinates.y));
	}
	else //Height is too small; fit to width
	{
		float scale = lengths.x / (float)w;
		float offset = ((float)lengths.y - (h * scale)) / 2.0; //Center by width
		map.loadMap(sf::Vector2u(1u, 1u), scale);
		map.setPosition(sf::Vector2f(coordinates.x, coordinates.y + offset));
	}
}



void WorldGenGUI::F_startGeneration()
{
	delete[] buffer; //Reset for new lengths
	delete[] elevation;
	delete[] moisture;
	delete[] climate;

	lengthX = 1472u; //************************ THIS IS TO BE MADE MORE FLEXIBLE IN THE FUTURE; GET CONFIG CHOSEN IN THE GUI
	lengthY = 736u;
	length = lengthX * lengthY;

	map = TileMap(lengthX, lengthY);
	this->fitToSpace(sf::Vector2f(0.f, 0.f), sf::Vector2f(368.f, 184.f));

	buffer = new char[length];
	elevation = new double[length];
	moisture = new double[length];
	climate = new double[length];

	progress = 0;
	lastProgress = 0;
	inProgress = true;

	auto gen = [this]()
	{
		generateEarth(lengthX, lengthY, "earth.bin", buffer, elevation, moisture, climate, inProgress, progress, 500u, &phone, true);
	};

	t2 = std::thread(gen); //Automatically starts execution
}

void WorldGenGUI::F_stopGeneration()
{
	phone.lock();
	if (inProgress)
		inProgress = false; //Abort
	phone.unlock();
	t2.join(); //Wait until task is aborted
}