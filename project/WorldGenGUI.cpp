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

void WorldGenGUI::initialize()
{
	map = TileMap(MAP_SCREEN_WIDTH, MAP_SCREEN_HEIGHT);
	this->fitToSpace(sf::Vector2f(MAP_SCREEN_X, MAP_SCREEN_Y), sf::Vector2f(MAP_SCREEN_WIDTH, MAP_SCREEN_HEIGHT)); //Start with a map at the maximum screen size


	tgui::Button::Ptr startBtn = tgui::Button::create("Generate");
	startBtn->setTextSize(19u);
	startBtn->setSize(105.f, 25.f);
	startBtn->setPosition(15.f, 200.f);
	startBtn->onPress([this]() { this->F_startGeneration(); });

	tgui::Button::Ptr cancelBtn = tgui::Button::create("Cancel");
	cancelBtn->setTextSize(19u);
	cancelBtn->setSize(95.f, 25.f);
	cancelBtn->setPosition(135.f, 200.f);
	cancelBtn->onPress([this]() { this->F_stopGeneration(); });
	
	tgui::EditBox::Ptr rs = tgui::EditBox::create();
	rs->setPosition(270.f, 200.f);
	rs->setDefaultText("");
	rs->onTextChange([this]() {
		auto RS = gui.get<tgui::EditBox>("rs");
		this->F_manageNumInput(RS, false, true);
	});

	gui.add(startBtn);
	gui.add(cancelBtn);
	gui.add(rs, "rs");
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



void WorldGenGUI::F_startGeneration()
{
	this->F_stopGeneration();

	delete[] buffer; //Reset for new lengths
	delete[] elevation;
	delete[] moisture;
	delete[] climate;

	lengthX = 1472u; //************************ THIS IS TO BE MADE MORE FLEXIBLE IN THE FUTURE; GET CONFIG CHOSEN IN THE GUI
	lengthY = 736u;
	length = lengthX * lengthY;

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
	if (t2.joinable())
		t2.join(); //Wait until task is aborted
}

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