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
	void handleEvents(sf::Event event);

private:

	tgui::Font mainFont; //To be used for all text

	const int MAP_SCREEN_WIDTH = 368; //The space the map gets
	const int MAP_SCREEN_HEIGHT = 184;
	const int MAP_SCREEN_X = 0;
	const int MAP_SCREEN_Y = 0;

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
	void setGlobals(); //Retrieve the GUI variables, and set the globals to equal them
	void updateVarSafely(std::string varName, tgui::String newText);

	void fitToSpace(sf::Vector2f coordinates, sf::Vector2f lengths);
	unsigned int transformSeed(std::string seed); //Converts a seed into a numerical value using a pseudorandom function

	void F_startGeneration();
	void F_stopGeneration();
	void F_manageNumInput(tgui::EditBox::Ptr box, bool forceInt = false, bool forceUnsigned = false); //Stops users from entering non-numerical input
	void F_scaleInput(tgui::EditBox::Ptr box, unsigned int limit); //Limits input to a certain amount of characters
};