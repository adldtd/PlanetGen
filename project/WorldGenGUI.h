#pragma once
#include "PlanetGenerator.h"
#include "GameObjects/ImageMap.h"
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

	WorldGenGUI(sf::RenderWindow* w);
	~WorldGenGUI();
	void setTarget(sf::RenderWindow* w);

	void update(); //Called by main; mainly checks for updates retrieved from the generation function
	void handleEvents(sf::Event& event);
	void draw();

private:

	tgui::GuiSFML gui; //Both are public for easier window access
	ImageMap map = ImageMap(0u, 0u); //Represents the image of the world to be drawn
	sf::RectangleShape background; //Image behind the GUI
	sf::RectangleShape displayBackground; //Colored rectangle behind the world display

	bool holdingF4;

	sf::RenderWindow* window;
	tgui::Font mainFont; //To be used for all text

	const int MAP_SCREEN_WIDTH = 368; //The space the map gets
	const int MAP_SCREEN_HEIGHT = 184;
	const int MAP_SCREEN_X = 0;
	const int MAP_SCREEN_Y = 0;

	std::thread t2;
	std::mutex phone;

	char* buffer = nullptr; //Have to be memory-managed throughout the class and widget functions
	float* elevation = nullptr;
	float* moisture = nullptr;
	float* climate = nullptr;

	unsigned int lengthX;
	unsigned int lengthY;
	unsigned int length;
	std::string seedString;

	int progress;
	int stage;
	int lastProgress;
	int lastStage;
	bool inProgress; //Variable passed to the generation function; remains true unless execution has to be stopped
	bool incomplete; //Variable that keeps track of when generation is terminated, not allowing a save

	void initialize(); //Called by constructor; sets up all of the window widgets
	void setGlobals(); //Retrieve the GUI variables, and set the globals to equal them
	void updateVarSafely(std::string varName, tgui::String newText);

	bool malloc_values(); //Instantiate buffer, elevation, moisture, and climate
	void free_values(); //Delete those values; free memory up

	void fillImageMap(); //Update every tile of map with buffer values
	unsigned char** export_color_map() const; //Creates an array of byte colors to be passed to the save function
	
	static unsigned int transformSeed(std::string seed); //Converts a seed into a numerical value using a pseudorandom function
	static std::string randomSeed(unsigned int limit); //Returns a string filled with random characters

	void F_startGeneration();
	void F_stopGeneration();
	void F_saveImage();
	void F_manageNumInput(tgui::EditBox::Ptr box, bool forceInt = false, bool forceUnsigned = false); //Stops users from entering non-numerical input
	void F_scaleInput(tgui::EditBox::Ptr box, unsigned int limit); //Limits input to a certain amount of characters
};