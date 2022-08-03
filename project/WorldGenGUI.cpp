#include "WorldGenGUI.h"
#include <TGUI/TGUI.hpp>
#include <SFML/Graphics.hpp>
#include "Globals.h"
#include "save.h"
#include <climits>

WorldGenGUI::WorldGenGUI(sf::RenderWindow* w)
{
	background.setSize(sf::Vector2f(736.f, 368.f));
	background.setPosition(sf::Vector2f(0.f, 0.f));
	background.setFillColor(sf::Color(194u, 194u, 194u, 255u));

	displayBackground.setSize(sf::Vector2f(MAP_SCREEN_WIDTH, MAP_SCREEN_HEIGHT));
	displayBackground.setPosition(sf::Vector2f(0.f, 0.f));
	displayBackground.setFillColor(sf::Color(0u, 0u, 0u, 255u));

	lengthX = MAP_SCREEN_WIDTH;
	lengthY = MAP_SCREEN_HEIGHT;
	length = lengthX * lengthY;

	holdingF4 = false;

	progress = 0;
	stage = 0;
	lastProgress = 0;
	lastStage = 0;
	inProgress = false;
	incomplete = true;

	this->setTarget(w);
	initialize();
}

WorldGenGUI::~WorldGenGUI()
{
	this->F_stopGeneration();
	this->free_values();
}

void WorldGenGUI::setTarget(sf::RenderWindow* w)
{
	window = w;
	gui.setTarget(*w);
}



/*********************************************************************************************
Retrieves values entered into the GUI and places them into the global values, which can then
be used as input to the generation function
*********************************************************************************************/
void WorldGenGUI::setGlobals()
{
	length_x = gui.get<tgui::EditBox>("widthBox")->getText().toUInt(0u);
	length_y = gui.get<tgui::EditBox>("heightBox")->getText().toUInt(0u);

	if (length_x != 0u) {
		unsigned int res = UINT_MAX / 4u; //length_x and length_y are passed into ImageMap, which has to create 4 vertices for each tile
		if (res < length_x)
			length_x = res;
	}

	unsigned int res = 0u;
	if (length_x != 0u)
		res = UINT_MAX / (length_x * 4);
	if (res < length_y) //Checks for possible uint overflow during multiplication
		length_y = res;

	loop_x = gui.get<tgui::ToggleButton>("loopXBtn")->isDown();
	loop_y = gui.get<tgui::ToggleButton>("loopYBtn")->isDown();

	seedString = gui.get<tgui::EditBox>("seedBox")->getText().toStdString();
	if (seedString == "")
		seedString = WorldGenGUI::randomSeed(19u);
	seed = WorldGenGUI::transformSeed(seedString);

	land_cruncher = gui.get<tgui::EditBox>("landCruncherBox")->getText().toFloat(1.f);
	land_point = gui.get<tgui::EditBox>("landPointBox")->getText().toFloat(0.f);
	sparsity = gui.get<tgui::EditBox>("landSparsityBox")->getText().toFloat(0.001f);
	if (sparsity == 0.f) sparsity = 0.001f; //Will lead to a division by 0 error

	island_cruncher = gui.get<tgui::EditBox>("islandCruncherBox")->getText().toFloat(1.f);
	i_point = gui.get<tgui::EditBox>("islandPointBox")->getText().toFloat(0.f);
	i_sparsity = gui.get<tgui::EditBox>("islandSparsityBox")->getText().toFloat(0.001f);
	if (i_sparsity == 0.f) i_sparsity = 0.001f;
	i_punish = gui.get<tgui::EditBox>("islandPunishmentBox")->getText().toFloat(0.f);

	climate_cruncher = gui.get<tgui::EditBox>("climateCruncherBox")->getText().toFloat(1.f);
	range_start = gui.get<tgui::EditBox>("climateRangeStartBox")->getText().toFloat(0.f);
	range_stop = gui.get<tgui::EditBox>("climateRangeStopBox")->getText().toFloat(1.f);
	max_modify = gui.get<tgui::EditBox>("climateMaxModifyBox")->getText().toFloat(0.f);
	using_equator = gui.get<tgui::ToggleButton>("useEquatorBtn")->isDown();

	moisture_cruncher = gui.get<tgui::EditBox>("moistureCruncherBox")->getText().toFloat(1.f);
	beach_point = gui.get<tgui::EditBox>("beachPointBox")->getText().toFloat(0.f);


	updateVarSafely("widthBox", tgui::String(length_x));
	updateVarSafely("heightBox", tgui::String(length_y));
	updateVarSafely("seedBox", seedString);

	updateVarSafely("landCruncherBox", tgui::String(land_cruncher));
	updateVarSafely("landPointBox", tgui::String(land_point));
	updateVarSafely("landSparsityBox", tgui::String(sparsity));

	updateVarSafely("islandCruncherBox", tgui::String(island_cruncher));
	updateVarSafely("islandPointBox", tgui::String(i_point));
	updateVarSafely("islandSparsityBox", tgui::String(i_sparsity));
	updateVarSafely("islandPunishmentBox", tgui::String(i_punish));

	updateVarSafely("climateCruncherBox", tgui::String(climate_cruncher));
	updateVarSafely("climateRangeStartBox", tgui::String(range_start));
	updateVarSafely("climateRangeStopBox", tgui::String(range_stop));
	updateVarSafely("climateMaxModifyBox", tgui::String(max_modify));

	updateVarSafely("moistureCruncherBox", tgui::String(moisture_cruncher));
	updateVarSafely("beachPointBox", tgui::String(beach_point));
}

/*********************************************************************************************
Updates a single EditBox without a text change event
*********************************************************************************************/
void WorldGenGUI::updateVarSafely(std::string varName, tgui::String newText)
{
	auto w = gui.get<tgui::EditBox>(varName);
	w->onTextChange.setEnabled(false);
	w->setDefaultText(newText); w->setText(newText);
	w->onTextChange.setEnabled(true);
}



/*********************************************************************************************
Called once every frame to check for and accommodate for changes
*********************************************************************************************/
void WorldGenGUI::update()
{
	phone.lock(); //Load pixels on tile map
	if (lastProgress < progress - 1 || lastStage < stage)
	{
		while (lastProgress < progress - 1 || lastStage < stage)
		{
			lastProgress++;
			int index = lastProgress % length;
			unsigned int tileX = index % lengthX; unsigned int tileY = index / lengthX;

			int coloring[4];
			returnRGBA(buffer[index], coloring);
			map.updateTile(sf::Vector2u(tileX, tileY), coloring[0], coloring[1], coloring[2], coloring[3]);

			if ((lastProgress == (length - 1)) && lastStage < stage)
			{
				lastProgress = 0;
				lastStage++;
			}
		}

		if (inProgress)
			lastProgress--; //Quick fix for a bug where the program sometimes "skips" certain pixels; lastProgress will always be less than progress
	}

	if (!inProgress && !incomplete)
		gui.get<tgui::Button>("saveBtn")->setEnabled(true);
	else
		gui.get<tgui::Button>("saveBtn")->setEnabled(false);

	phone.unlock();
}

void WorldGenGUI::draw()
{
	window->draw(background);
	window->draw(displayBackground);
	window->draw(map);
	if (!holdingF4) gui.draw();
}



void WorldGenGUI::handleEvents(sf::Event& event)
{
	if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F4)
	{
		if (!holdingF4)
		{
			displayBackground.setScale(sf::Vector2f(2.f, 2.f));
			//map = ImageMap(lengthX, lengthY); //**************************************FOR ONE FRAME, THIS IS TOO EXPENSIVE!!!!!!!!!
			map.fitToSpace(sf::Vector2f(MAP_SCREEN_X, MAP_SCREEN_Y), sf::Vector2f(MAP_SCREEN_WIDTH * 2.f, MAP_SCREEN_HEIGHT * 2.f), 0, 0, 130, 255, true);
			this->fillImageMap();

			gui.unfocusAllWidgets();
			gui.get<tgui::Button>("startBtn")->onPress.setEnabled(false);
			gui.get<tgui::Button>("cancelBtn")->onPress.setEnabled(false);
			gui.get<tgui::Button>("saveBtn")->onPress.setEnabled(false);
			holdingF4 = true;
		}
	}
	else if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::F4)
	{
		if (holdingF4)
		{
			displayBackground.setScale(sf::Vector2f(1.f, 1.f));
			//map = ImageMap(lengthX, lengthY);
			map.fitToSpace(sf::Vector2f(MAP_SCREEN_X, MAP_SCREEN_Y), sf::Vector2f(MAP_SCREEN_WIDTH, MAP_SCREEN_HEIGHT), 0, 0, 130, 255, true);
			this->fillImageMap();

			gui.unfocusAllWidgets();
			gui.get<tgui::Button>("startBtn")->onPress.setEnabled(true);
			gui.get<tgui::Button>("cancelBtn")->onPress.setEnabled(true);
			gui.get<tgui::Button>("saveBtn")->onPress.setEnabled(true);
			holdingF4 = false;
		}
	}

	gui.handleEvent(event);
}

/*********************************************************************************************
Fills map up with all data currently retrieved
*********************************************************************************************/
void WorldGenGUI::fillImageMap()
{
	phone.lock();

	unsigned int limit;
	if (stage == 0) //The generator has not created all of the sea + continents yet
		limit = progress;
	else
		limit = length;

	for (unsigned int i = 0; i < limit; i++)
	{
		unsigned int tileX = i % lengthX; unsigned int tileY = i / lengthX;
		int coloring[4] = { 0, 0, 0, 255 };
		returnRGBA(buffer[i], coloring);
		map.updateTile(sf::Vector2u(tileX, tileY), coloring[0], coloring[1], coloring[2], coloring[3]);
	}
	phone.unlock();
}

/*********************************************************************************************
Creates a 2D RGBA byte array to pass to the save function; should not be callable while the
generator is still running
*********************************************************************************************/
unsigned char** WorldGenGUI::export_color_map() const
{
	unsigned char** colors;
	try
	{
		colors = new unsigned char* [lengthY];
	}
	catch (std::bad_alloc e)
	{
		std::cout << "Could not allocate enough memory for image data" << std::endl;
		return NULL;
	}

	unsigned int r = 0;
	for (unsigned int r = 0; r < lengthY; r++)
	{
		try
		{
			colors[r] = new unsigned char[lengthX * 4];
		}
		catch (std::bad_alloc e)
		{
			std::cout << "Could not allocate enough memory for row " << r << " of the image data" << std::endl;
			for (unsigned int j = 0; j < r; j++)
				delete[] colors[j];
			delete[] colors;
			return NULL;
		}

		for (unsigned int i = 0; i < lengthX; i++)
		{
			unsigned int bufferIndex = i + (r * lengthX);
			unsigned int byteIndex = i * 4;

			int coloring[4];
			returnRGBA(buffer[bufferIndex], coloring);

			colors[r][byteIndex] = coloring[0];
			colors[r][byteIndex + 1] = coloring[1];
			colors[r][byteIndex + 2] = coloring[2];
			colors[r][byteIndex + 3] = coloring[3];
		}
	}

	return colors;
}



/*********************************************************************************************
Transforms the seed entered by the user into an unsigned int, which can be inputted into the
generator function
*********************************************************************************************/
unsigned int WorldGenGUI::transformSeed(std::string seed)
{
	unsigned int returnVal = 0u;
	auto iter = seed.begin();
	srand(0u);

	while (iter != seed.end())
	{
		int charVal = static_cast<int>(*iter);
		returnVal += charVal + (rand() * rand());
		srand(returnVal);
		iter++;
	}

	std::cout << returnVal << std::endl;
	return returnVal;
}

/*********************************************************************************************
Returns a string filled with a number (defined as limit) of random characters
*********************************************************************************************/
std::string WorldGenGUI::randomSeed(unsigned int limit)
{
	std::string returnStr;

	auto t = std::chrono::system_clock::now();
	srand(t.time_since_epoch().count()); //Almost completely random

	for (unsigned int c = 0; c < limit; c++)
	{
		int newc = (rand() % 94) + 33;
		returnStr.push_back(static_cast<char>(newc)); //From '!' to '~'
	}

	return returnStr;
}



bool WorldGenGUI::malloc_values()
{
	bool returnVal = true;

	try
	{
		buffer = new char[length];
		elevation = new float[length];
		moisture = new float[length];
		climate = new float[length];
	}
	catch (std::bad_alloc& e)
	{
		std::cout << "Could not allocate enough memory for the map data: " << e.what() << std::endl;
		this->free_values();
		returnVal = false;
	}

	return returnVal;
}

void WorldGenGUI::free_values()
{
	if (buffer != nullptr)
	{
		delete[] buffer;
		buffer = nullptr;
	}
	if (elevation != nullptr)
	{
		delete[] elevation;
		elevation = nullptr;
	}
	if (moisture != nullptr)
	{
		delete[] moisture;
		moisture = nullptr;
	}
	if (climate != nullptr)
	{
		delete[] climate;
		climate = nullptr;
	}
}



/*********************************************************************************************
Stops any currently running threads, performs memory management, and begins a new thread
with the generation function
*********************************************************************************************/
void WorldGenGUI::F_startGeneration()
{
	this->F_stopGeneration();
	this->free_values(); //Reset for new lengths
	gui.get<tgui::Button>("saveBtn")->setEnabled(false);

	this->setGlobals(); //Retrieve all information entered into the GUI

	lengthX = length_x;
	lengthY = length_y;
	length = lengthX * lengthY;

	if (length == 0)
	{
		incomplete = true;
		return;
	}

	map = ImageMap(lengthX, lengthY);
	if (map.getHeight() == 0u || map.getWidth() == 0u)
	{
		lastProgress = progress;
		incomplete = true;
		return;
	}
	map.fitToSpace(sf::Vector2f(MAP_SCREEN_X, MAP_SCREEN_Y), sf::Vector2f(MAP_SCREEN_WIDTH, MAP_SCREEN_HEIGHT), 0, 0, 130, 255, true); //This function loads the map


	if (!this->malloc_values())
	{
		lastProgress = progress;
		incomplete = true;
		return;
	}

	progress = 0;
	stage = 0;
	lastProgress = 0;
	lastStage = 0;
	inProgress = true;
	incomplete = false;

	auto gen = [this]()
	{
		generateEarth(lengthX, lengthY, "earth.bin", buffer, elevation, moisture, climate, inProgress, progress, stage, seed, &phone, true);
	};

	t2 = std::thread(gen); //Automatically starts execution
}

/*********************************************************************************************
Flags world generation to stop, and waits for the thread to end
*********************************************************************************************/
void WorldGenGUI::F_stopGeneration()
{
	phone.lock();
	if (inProgress)
	{
		inProgress = false; //Abort
		incomplete = true; //Unsaveable
	}
	phone.unlock();
	if (t2.joinable())
	{
		t2.join(); //Wait until task is aborted
		std::cout << "Cancelled" << std::endl;
	}
}

/*********************************************************************************************
Saves the loaded map as a PNG image
*********************************************************************************************/
void WorldGenGUI::F_saveImage()
{
	phone.lock();
	if (inProgress)
	{
		phone.unlock();
		return;
	}
	phone.unlock();

	unsigned char** colors = this->export_color_map();
	if (!colors)
	{
		std::cout << "The map could not be saved." << std::endl;
		return;
	}

	if (!save(colors, seedString.c_str(), lengthX, lengthY))
		std::cout << "The map could not be saved." << std::endl;
}

/*********************************************************************************************
Helper function for a numerical EditBox; forces input to stay in number form
*********************************************************************************************/
void WorldGenGUI::F_manageNumInput(tgui::EditBox::Ptr box, bool forceInt, bool forceUnsigned)
{
	bool invalid = false;

	auto iter = box->getText().begin();

	bool firstChar = true;
	bool lastWasSign = false;
	bool wasDecimalPoint = false;

	while (iter != box->getText().end() && !invalid)
	{
		if (std::isdigit(*iter))
		{
			iter++;
			lastWasSign = false;
		}
		else if (*iter == '.' && !forceInt && !lastWasSign && !wasDecimalPoint)
		{
			iter++;
			wasDecimalPoint = true;
			lastWasSign = false;
		}
		else if (*iter == '-' && !forceUnsigned && firstChar)
		{
			iter++;
			lastWasSign = true;
		}
		else
		{
			invalid = true;
		}

		firstChar = false;
	}

	box->onTextChange.setEnabled(false);
	if (invalid)
		box->setText(box->getDefaultText()); //Default text stores the last valid string entered by the user
	else
		box->setDefaultText(box->getText()); //Text is accepted
	box->onTextChange.setEnabled(true);
}

/*********************************************************************************************
Forces EditBox input to a certain character size
*********************************************************************************************/
void WorldGenGUI::F_scaleInput(tgui::EditBox::Ptr box, unsigned int limit)
{
	box->onTextChange.setEnabled(false);
	if (box->getText().length() > limit)
		box->setText(box->getDefaultText());
	else
		box->setDefaultText(box->getText());
	box->onTextChange.setEnabled(true);
}