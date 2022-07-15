#pragma once
#include "PlanetGenerator.h"
#include "GameObjects/TileMap.h"
#include <thread>
#include <TGUI/TGUI.hpp>
#include <mutex>
#include <SFML/Graphics.hpp>


/*********************************************************************************************
A wrapper class for the GUI display for a window
*********************************************************************************************/
class WorldGenGUI
{
public:

	tgui::GuiSFML gui; //Both are public for easier window access
	TileMap map = TileMap(0u, 0u); //Represents the image of the world to be drawn

	WorldGenGUI();
	~WorldGenGUI();
	void update(); //Called by main; mainly checks for updates retrieved from the generation function

private:

	std::thread t2;
	std::mutex phone;

	char* buffer = nullptr; //Have to be memory-managed throughout the class and widget functions
	double* elevation = nullptr;
	double* moisture = nullptr;
	double* climate = nullptr;

	unsigned int lengthX;
	unsigned int lengthY;
	unsigned int length;

	int progress;
	int lastProgress;
	bool inProgress; //Variable passed to the generation function; remains true unless execution has to be stopped

	void initialize(); //Called by constructor; sets up all of the window widgets 
	void fitToSpace(sf::Vector2f coordinates, sf::Vector2f lengths);

	void F_startGeneration();
	void F_stopGeneration();
};