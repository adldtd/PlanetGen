#include "WorldGenGUI.h"
#include <TGUI/TGUI.hpp>
#include <SFML/Graphics.hpp>
#include "Globals.h"

WorldGenGUI::WorldGenGUI()
{
	progress = 0;
	lastProgress = 0;
	inProgress = false;

	initialize();
}

WorldGenGUI::~WorldGenGUI()
{
	this->F_stopGeneration();

	if (buffer != nullptr) delete[] buffer;
	if (elevation != nullptr) delete[] elevation;
	if (moisture != nullptr) delete[] moisture;
	if (climate != nullptr) delete[] climate;
}



/*********************************************************************************************
Retrieves values entered into the GUI and places them into the global values, which can then
be used as input to the generation function
*********************************************************************************************/
void WorldGenGUI::setGlobals()
{
	using namespace guivars;

	auto WB = gui.get<tgui::EditBox>("widthBox");
	length_x = WB->getText().toUInt(0u);

	auto HB = gui.get<tgui::EditBox>("heightBox");
	length_y = HB->getText().toUInt(0u);

	auto SB = gui.get<tgui::EditBox>("seedBox");
	seed = this->transformSeed(SB->getText().toStdString());
}



/*********************************************************************************************
Called once every frame to check for and accommodate for changes
*********************************************************************************************/
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

void WorldGenGUI::handleEvents(sf::Event event)
{
	gui.handleEvent(event);
}



/*********************************************************************************************
Transforms the seed entered by the user into an unsigned int, which can be inputted into the
generator function
*********************************************************************************************/
unsigned int WorldGenGUI::transformSeed(std::string seed)
{
	unsigned int returnVal = 0u;
	auto iter = seed.begin();

	while (iter != seed.end())
	{
		int charVal = static_cast<int>(*iter);
		srand(charVal);
		returnVal += (charVal % 7) * (charVal % 11) + (charVal % (rand() % 17));
		iter++;
	}

	std::cout << returnVal << std::endl;
	return returnVal;
}

/*********************************************************************************************
Fits a generated map to an enclosed display space
*********************************************************************************************/
void WorldGenGUI::fitToSpace(sf::Vector2f coordinates, sf::Vector2f lengths)
{
	int w = map.getWidth(); //Each is measured in pixels
	int h = map.getHeight();

	float idealFactor = lengths.x / (float)lengths.y;
	float tileFactor = w / (float)h;

	if (tileFactor == idealFactor) //Fits perfectly
	{
		float scale = lengths.x / (float)w;
		map.loadMap(sf::Vector2u(1u, 1u), 0, 0, 130, 255, scale);
		map.setPosition(coordinates);
	}
	else if (tileFactor < idealFactor) //Width is too small; fit to height
	{
		float scale = lengths.y / (float)h;
		float offset = ((float)lengths.x - (w * scale)) / 2.0; //Center by width
		map.loadMap(sf::Vector2u(1u, 1u), 0, 0, 130, 255, scale);
		map.setPosition(sf::Vector2f(coordinates.x + offset, coordinates.y));
	}
	else //Height is too small; fit to width
	{
		float scale = lengths.x / (float)w;
		float offset = ((float)lengths.y - (h * scale)) / 2.0; //Center by width
		map.loadMap(sf::Vector2u(1u, 1u), 0, 0, 130, 255, scale);
		map.setPosition(sf::Vector2f(coordinates.x, coordinates.y + offset));
	}
}



/*********************************************************************************************
Stops any currently running threads, performs memory management, and begins a new thread
with the generation function
*********************************************************************************************/
void WorldGenGUI::F_startGeneration()
{
	this->F_stopGeneration();

	if (buffer != nullptr) delete[] buffer; //Reset for new lengths
	if (elevation != nullptr) delete[] elevation;
	if (moisture != nullptr) delete[] moisture;
	if (climate != nullptr) delete[] climate;

	this->setGlobals(); //Retrieve all information entered into the GUI

	//lengthX = 1472u;
	//lengthY = 736u;
	lengthX = guivars::length_x;
	lengthY = guivars::length_y;
	length = lengthX * lengthY;

	if (length == 0)
		return;

	map = TileMap(lengthX, lengthY);
	this->fitToSpace(sf::Vector2f(MAP_SCREEN_X, MAP_SCREEN_Y), sf::Vector2f(MAP_SCREEN_WIDTH, MAP_SCREEN_HEIGHT)); //This function loads the map

	buffer = new char[length];
	elevation = new double[length];
	moisture = new double[length];
	climate = new double[length];

	progress = 0;
	lastProgress = 0;
	inProgress = true;

	auto gen = [this]()
	{
		generateEarth(lengthX, lengthY, "earth.bin", buffer, elevation, moisture, climate, inProgress, progress, guivars::seed, &phone, true);
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
		inProgress = false; //Abort
	phone.unlock();
	if (t2.joinable())
		t2.join(); //Wait until task is aborted
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