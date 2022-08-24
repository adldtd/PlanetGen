#include "WorldGenGUI.h"
#include <TGUI/TGUI.hpp>
#include <SFML/Graphics.hpp>

void registerHeader(tgui::EditBox::Ptr header);


/*********************************************************************************************
Entirely responsible for the layout, styling, and functionality of the GUI upon loading
*********************************************************************************************/
void WorldGenGUI::initialize()
{
	mainFont = tgui::Font("consola.ttf");
	gui.setFont(mainFont);

	map = ImageMap(MAP_SCREEN_WIDTH, MAP_SCREEN_HEIGHT);
	map.fitToSpace(sf::Vector2f(MAP_SCREEN_X, MAP_SCREEN_Y), sf::Vector2f(MAP_SCREEN_WIDTH, MAP_SCREEN_HEIGHT), 0, 0, 130, 255, true); //Start with a map at the maximum screen size

	//********************************************************************************************* CONSOLE GUI GROUP

	tgui::Theme::Ptr theme = tgui::Theme::create("themes/InnerConsole.txt");

	tgui::ChatBox::Ptr consoleBox = tgui::ChatBox::create();
	consoleBox->setRenderer(theme->getRenderer("ChatBox"));
	consoleBox->setSize(335.f, 100.f);
	consoleBox->setPosition(15.f, 235.f);
	consoleBox->setLineLimit(INNER_CONSOLE_MAX_LINES);
	consoleBox->setTextSize(11u);
	consoleBox->setTextColor(tgui::Color::White);
	innerConsole = consoleBox;

	tgui::Button::Ptr flushBtn = tgui::Button::create("Flush");
	flushBtn->setSize(55.f, 16.f);
	flushBtn->setPosition(295.f, 344.f);
	flushBtn->setTextSize(11u);
	flushBtn->onPress([this]() { this->F_flush(); });

	//********************************************************************************************* GENERATION GUI GROUP

	tgui::Button::Ptr startBtn = tgui::Button::create("Generate");
	startBtn->setTextSize(19u);
	startBtn->setSize(105.f, 25.f);
	startBtn->setPosition(15.f, 200.f);
	startBtn->onPress([this]() { this->F_startGeneration(); });

	tgui::Button::Ptr cancelBtn = tgui::Button::create("Cancel");
	cancelBtn->setTextSize(19u);
	cancelBtn->setSize(95.f, 25.f);
	cancelBtn->setPosition(140.f, 200.f);
	cancelBtn->onPress([this]()
	{ this->F_stopGeneration(); });

	tgui::Button::Ptr saveBtn = tgui::Button::create("Save");
	saveBtn->setTextSize(19u);
	saveBtn->setSize(95.f, 25.f);
	saveBtn->setPosition(255.f, 200.f);
	saveBtn->onPress([this]() { this->F_saveImage(); });
	saveBtn->setEnabled(false);

	//********************************************************************************************* WORLD META GUI GROUP

	tgui::EditBox::Ptr worldArea = tgui::EditBox::create();
	worldArea->setReadOnly();
	worldArea->setSize(40.f, 16.f);
	worldArea->setPosition(375.f, 2.f);
	worldArea->setText("World");
	worldArea->setTextSize(9u);

	tgui::TextArea::Ptr worldInfo = tgui::TextArea::create();
	worldInfo->setSize(145.f, 27.f);
	worldInfo->addText("Meta information for WorldGen");
	worldInfo->setTextSize(9u);
	worldInfo->setReadOnly();
	worldArea->setToolTip(worldInfo);


	tgui::EditBox::Ptr loopXArea = tgui::EditBox::create();
	registerHeader(loopXArea);
	loopXArea->setPosition(519.f, 2.f);
	loopXArea->setText("Hori. Loop");

	tgui::TextArea::Ptr loopXInfo = tgui::TextArea::create();
	loopXInfo->setSize(145.f, 27.f);
	loopXInfo->addText("When enabled, the map loops around horizontally");
	loopXInfo->setTextSize(9u);
	loopXInfo->setReadOnly();
	loopXArea->setToolTip(loopXInfo);

	tgui::ToggleButton::Ptr loopXBtn = tgui::ToggleButton::create();
	loopXBtn->setSize(16.f, 16.f);
	loopXBtn->setPosition(504.f, 2.f);


	tgui::EditBox::Ptr loopYArea = tgui::EditBox::create();
	registerHeader(loopYArea);
	loopYArea->setPosition(620.f, 2.f);
	loopYArea->setText("Verti. Loop");

	tgui::TextArea::Ptr loopYInfo = tgui::TextArea::create();
	loopYInfo->setSize(145.f, 27.f);
	loopYInfo->addText("When enabled, the map loops around vertically");
	loopYInfo->setTextSize(9u);
	loopYInfo->setReadOnly();
	loopYArea->setToolTip(loopYInfo);

	tgui::ToggleButton::Ptr loopYBtn = tgui::ToggleButton::create();
	loopYBtn->setSize(16.f, 16.f);
	loopYBtn->setPosition(605.f, 2.f);


	tgui::EditBox::Ptr widthArea = tgui::EditBox::create();
	registerHeader(widthArea);
	widthArea->setPosition(388.f, 23.f);
	widthArea->setText("Map Width");

	tgui::TextArea::Ptr widthInfo = tgui::TextArea::create();
	widthInfo->setSize(145.f, 16.f);
	widthInfo->addText("The map's pixel width");
	widthInfo->setTextSize(9u);
	widthInfo->setReadOnly();
	widthArea->setToolTip(widthInfo);

	tgui::EditBox::Ptr widthBox = tgui::EditBox::create();
	widthBox->setSize(70.f, 25.f);
	widthBox->setPosition(388.f, 40.f);
	widthBox->setDefaultText("1472");
	widthBox->setText("1472");
	widthBox->onTextChange([this]() {
		auto WB = gui.get<tgui::EditBox>("widthBox");
		this->F_manageNumInput(WB, true, true);
	});


	tgui::EditBox::Ptr heightArea = tgui::EditBox::create();
	registerHeader(heightArea);
	heightArea->setPosition(480.f, 23.f);
	heightArea->setText("Map Height");

	tgui::TextArea::Ptr heightInfo = tgui::TextArea::create();
	heightInfo->setSize(145.f, 16.f);
	heightInfo->addText("The map's pixel height");
	heightInfo->setTextSize(9u);
	heightInfo->setReadOnly();
	heightArea->setToolTip(heightInfo);

	tgui::EditBox::Ptr heightBox = tgui::EditBox::create();
	heightBox->setSize(70.f, 25.f);
	heightBox->setPosition(480.f, 40.f);
	heightBox->setDefaultText("736");
	heightBox->setText("736");
	heightBox->onTextChange([this]() {
		auto HB = gui.get<tgui::EditBox>("heightBox");
		this->F_manageNumInput(HB, true, true);
	});


	tgui::EditBox::Ptr seedArea = tgui::EditBox::create();
	registerHeader(seedArea);
	seedArea->setPosition(572.f, 23.f);
	seedArea->setText("Seed");

	tgui::EditBox::Ptr seedBox = tgui::EditBox::create();
	seedBox->setSize(140.f, 25.f);
	seedBox->setPosition(572.f, 40.f);
	seedBox->setDefaultText("");
	seedBox->onTextChange([this]() {
		auto SB = gui.get<tgui::EditBox>("seedBox");
		this->F_scaleInput(SB, 19);
	});

	//********************************************************************************************* CONTINENTAL GUI GROUP

	tgui::EditBox::Ptr continentalArea = tgui::EditBox::create(); //Distance of 35.f
	continentalArea->setReadOnly();
	continentalArea->setSize(70.f, 16.f);
	continentalArea->setPosition(375.f, 75.f);
	continentalArea->setText("Continental");
	continentalArea->setTextSize(9u);

	tgui::TextArea::Ptr continentalInfo = tgui::TextArea::create();
	continentalInfo->setSize(145.f, 27.f);
	continentalInfo->addText("Continent and ocean generation");
	continentalInfo->setTextSize(9u);
	continentalInfo->setReadOnly();
	continentalArea->setToolTip(continentalInfo);


	tgui::EditBox::Ptr landCruncherArea = tgui::EditBox::create(); //Distance of 21.f
	registerHeader(landCruncherArea);
	landCruncherArea->setPosition(388.f, 96.f);
	landCruncherArea->setText("Cruncher");

	tgui::TextArea::Ptr landCruncherInfo = tgui::TextArea::create();
	landCruncherInfo->setSize(145.f, 27.f);
	landCruncherInfo->addText("As the value increases, continents become beefier");
	landCruncherInfo->setTextSize(9u);
	landCruncherInfo->setReadOnly();
	landCruncherArea->setToolTip(landCruncherInfo);

	tgui::EditBox::Ptr landCruncherBox = tgui::EditBox::create(); //Distance of 17.f
	landCruncherBox->setSize(70.f, 25.f);
	landCruncherBox->setPosition(388.f, 113.f);
	landCruncherBox->setDefaultText("1.0");
	landCruncherBox->setText("1.0");
	landCruncherBox->onTextChange([this]() {
		auto w = gui.get<tgui::EditBox>("landCruncherBox");
		this->F_manageNumInput(w, false, true);
	});


	tgui::EditBox::Ptr landPointArea = tgui::EditBox::create();
	registerHeader(landPointArea);
	landPointArea->setPosition(480.f, 96.f);
	landPointArea->setText("Threshold");

	tgui::TextArea::Ptr landPointInfo = tgui::TextArea::create();
	landPointInfo->setSize(145.f, 44.f);
	landPointInfo->addText("Specifies at what point water becomes land. Range is between -1.0 to 1.0");
	landPointInfo->setTextSize(9u);
	landPointInfo->setReadOnly();
	landPointArea->setToolTip(landPointInfo);

	tgui::EditBox::Ptr landPointBox = tgui::EditBox::create();
	landPointBox->setSize(70.f, 25.f);
	landPointBox->setPosition(480.f, 113.f);
	landPointBox->setDefaultText("0.30");
	landPointBox->setText("0.30");
	landPointBox->onTextChange([this]() {
		auto w = gui.get<tgui::EditBox>("landPointBox");
		this->F_manageNumInput(w, false, false);
	});


	tgui::EditBox::Ptr landSparsityArea = tgui::EditBox::create();
	registerHeader(landSparsityArea);
	landSparsityArea->setPosition(572.f, 96.f);
	landSparsityArea->setText("Sparsity");

	tgui::TextArea::Ptr landSparsityInfo = tgui::TextArea::create();
	landSparsityInfo->setSize(145.f, 41.f);
	landSparsityInfo->addText("As the value increases, both the ocean and continent sizes increase");
	landSparsityInfo->setTextSize(9u);
	landSparsityInfo->setReadOnly();
	landSparsityArea->setToolTip(landSparsityInfo);

	tgui::EditBox::Ptr landSparsityBox = tgui::EditBox::create();
	landSparsityBox->setSize(70.f, 25.f);
	landSparsityBox->setPosition(572.f, 113.f);
	landSparsityBox->setDefaultText("350.0");
	landSparsityBox->setText("350.0");
	landSparsityBox->onTextChange([this]() {
		auto w = gui.get<tgui::EditBox>("landSparsityBox");
		this->F_manageNumInput(w, false, true);
	});

	//********************************************************************************************* ISLAND GUI GROUP

	tgui::EditBox::Ptr islandArea = tgui::EditBox::create(); //Distance of 35.f
	islandArea->setReadOnly();
	islandArea->setSize(50.f, 16.f);
	islandArea->setPosition(375.f, 148.f);
	islandArea->setText("Islands");
	islandArea->setTextSize(9u);

	tgui::TextArea::Ptr islandInfo = tgui::TextArea::create();
	islandInfo->setSize(145.f, 16.f);
	islandInfo->addText("Island generation");
	islandInfo->setTextSize(9u);
	islandInfo->setReadOnly();
	islandArea->setToolTip(islandInfo);


	tgui::EditBox::Ptr islandCruncherArea = tgui::EditBox::create(); //Distance of 21.f
	registerHeader(islandCruncherArea);
	islandCruncherArea->setPosition(388.f, 169.f);
	islandCruncherArea->setText("Cruncher");

	tgui::TextArea::Ptr islandCruncherInfo = tgui::TextArea::create();
	islandCruncherInfo->setSize(145.f, 27.f);
	islandCruncherInfo->addText("As the value increases, islands become beefier");
	islandCruncherInfo->setTextSize(9u);
	islandCruncherInfo->setReadOnly();
	islandCruncherArea->setToolTip(islandCruncherInfo);

	tgui::EditBox::Ptr islandCruncherBox = tgui::EditBox::create(); //Distance of 17.f
	islandCruncherBox->setSize(70.f, 25.f);
	islandCruncherBox->setPosition(388.f, 186.f);
	islandCruncherBox->setDefaultText("1.0");
	islandCruncherBox->setText("1.0");
	islandCruncherBox->onTextChange([this]() {
		auto w = gui.get<tgui::EditBox>("islandCruncherBox");
		this->F_manageNumInput(w, false, true);
	});


	tgui::EditBox::Ptr islandPointArea = tgui::EditBox::create();
	registerHeader(islandPointArea);
	islandPointArea->setPosition(472.f, 169.f);
	islandPointArea->setText("Spawn Area");

	tgui::TextArea::Ptr islandPointInfo = tgui::TextArea::create();
	islandPointInfo->setSize(145.f, 84.f);
	islandPointInfo->addText("Specifies at what point (in the water) islands are most likely to spawn. If value is bigger than the land threshold, no islands will spawn. Range is between -1.0 to 1.0");
	islandPointInfo->setTextSize(9u);
	islandPointInfo->setReadOnly();
	islandPointArea->setToolTip(islandPointInfo);

	tgui::EditBox::Ptr islandPointBox = tgui::EditBox::create();
	islandPointBox->setSize(70.f, 25.f);
	islandPointBox->setPosition(472.f, 186.f);
	islandPointBox->setDefaultText("-0.01");
	islandPointBox->setText("-0.01");
	islandPointBox->onTextChange([this]() {
		auto w = gui.get<tgui::EditBox>("islandPointBox");
		this->F_manageNumInput(w, false, false);
	});


	tgui::EditBox::Ptr islandSparsityArea = tgui::EditBox::create();
	registerHeader(islandSparsityArea);
	islandSparsityArea->setPosition(556.f, 169.f);
	islandSparsityArea->setText("Sparsity");

	tgui::TextArea::Ptr islandSparsityInfo = tgui::TextArea::create();
	islandSparsityInfo->setSize(145.f, 41.f);
	islandSparsityInfo->addText("As the value increases, island size increases, but distribution decreases");
	islandSparsityInfo->setTextSize(9u);
	islandSparsityInfo->setReadOnly();
	islandSparsityArea->setToolTip(islandSparsityInfo);

	tgui::EditBox::Ptr islandSparsityBox = tgui::EditBox::create();
	islandSparsityBox->setSize(70.f, 25.f);
	islandSparsityBox->setPosition(556.f, 186.f);
	islandSparsityBox->setDefaultText("50.0");
	islandSparsityBox->setText("50.0");
	islandSparsityBox->onTextChange([this]() {
		auto w = gui.get<tgui::EditBox>("islandSparsityBox");
		this->F_manageNumInput(w, false, true);
	});


	tgui::EditBox::Ptr islandPunishmentArea = tgui::EditBox::create();
	registerHeader(islandPunishmentArea);
	islandPunishmentArea->setPosition(640.f, 169.f);
	islandPunishmentArea->setText("Submergement");

	tgui::TextArea::Ptr islandPunishmentInfo = tgui::TextArea::create();
	islandPunishmentInfo->setSize(145.f, 54.f);
	islandPunishmentInfo->addText("As the value increases, islands become more \"submerged\" the farther they stray from their spawn area");
	islandPunishmentInfo->setTextSize(9u);
	islandPunishmentInfo->setReadOnly();
	islandPunishmentArea->setToolTip(islandPunishmentInfo);

	tgui::EditBox::Ptr islandPunishmentBox = tgui::EditBox::create();
	islandPunishmentBox->setSize(70.f, 25.f);
	islandPunishmentBox->setPosition(640.f, 186.f);
	islandPunishmentBox->setDefaultText("12.5");
	islandPunishmentBox->setText("12.5");
	islandPunishmentBox->onTextChange([this]() {
		auto w = gui.get<tgui::EditBox>("islandPunishmentBox");
		this->F_manageNumInput(w, false, false);
	});

	//********************************************************************************************* CLIMATE GUI GROUP

	tgui::EditBox::Ptr climateArea = tgui::EditBox::create(); //Distance of 35.f
	climateArea->setReadOnly();
	climateArea->setSize(50.f, 16.f);
	climateArea->setPosition(375.f, 221.f);
	climateArea->setText("Climate");
	climateArea->setTextSize(9u);

	tgui::TextArea::Ptr climateInfo = tgui::TextArea::create();
	climateInfo->setSize(145.f, 27.f);
	climateInfo->addText("Heat generation. First piece of biome generation");
	climateInfo->setTextSize(9u);
	climateInfo->setReadOnly();
	climateArea->setToolTip(climateInfo);


	tgui::EditBox::Ptr useEquatorArea = tgui::EditBox::create();
	registerHeader(useEquatorArea);
	useEquatorArea->setPosition(620.f, 221.f);
	useEquatorArea->setText("Use Equator");

	tgui::TextArea::Ptr useEquatorInfo = tgui::TextArea::create();
	useEquatorInfo->setSize(145.f, 77.f);
	useEquatorInfo->addText("When enabled, the program will attempt to \"center\" heat on a vertical middle line, moving from [generated value] + 0 to [generated value] + [max change]");
	useEquatorInfo->setTextSize(9u);
	useEquatorInfo->setReadOnly();
	useEquatorArea->setToolTip(useEquatorInfo);

	tgui::ToggleButton::Ptr useEquatorBtn = tgui::ToggleButton::create();
	useEquatorBtn->setSize(16.f, 16.f);
	useEquatorBtn->setPosition(605.f, 221.f);
	useEquatorBtn->onClick([this]() {
		auto w = gui.get<tgui::ToggleButton>("useEquatorBtn");
		auto w1 = gui.get<tgui::EditBox>("climateMaxModifyBox");
		auto w2 = gui.get<tgui::EditBox>("climateMaxModifyArea");
		w1->setEnabled(w->isDown()); w2->setEnabled(w->isDown());
	});


	tgui::EditBox::Ptr climateCruncherArea = tgui::EditBox::create(); //Distance of 21.f
	registerHeader(climateCruncherArea);
	climateCruncherArea->setPosition(388.f, 242.f);
	climateCruncherArea->setText("Cruncher");

	tgui::TextArea::Ptr climateCruncherInfo = tgui::TextArea::create();
	climateCruncherInfo->setSize(145.f, 40.f);
	climateCruncherInfo->addText("As the value increases, the climate becomes more \"extreme\"");
	climateCruncherInfo->setTextSize(9u);
	climateCruncherInfo->setReadOnly();
	climateCruncherArea->setToolTip(climateCruncherInfo);

	tgui::EditBox::Ptr climateCruncherBox = tgui::EditBox::create(); //Distance of 17.f
	climateCruncherBox->setSize(70.f, 25.f);
	climateCruncherBox->setPosition(388.f, 259.f);
	climateCruncherBox->setDefaultText("1.0");
	climateCruncherBox->setText("1.0");
	climateCruncherBox->onTextChange([this]() {
		auto w = gui.get<tgui::EditBox>("climateCruncherBox");
		this->F_manageNumInput(w, false, true);
	});


	tgui::EditBox::Ptr climateRangeStartArea = tgui::EditBox::create();
	registerHeader(climateRangeStartArea);
	climateRangeStartArea->setPosition(472.f, 242.f);
	climateRangeStartArea->setText("Range Start");

	tgui::TextArea::Ptr climateRangeStartInfo = tgui::TextArea::create();
	climateRangeStartInfo->setSize(145.f, 54.f);
	climateRangeStartInfo->addText("Valid range is between 0 and 1.0. 0 (and lower) is extreme cold, while 1.0 (and higher) is extreme heat");
	climateRangeStartInfo->setTextSize(9u);
	climateRangeStartInfo->setReadOnly();
	climateRangeStartArea->setToolTip(climateRangeStartInfo);

	tgui::EditBox::Ptr climateRangeStartBox = tgui::EditBox::create();
	climateRangeStartBox->setSize(70.f, 25.f);
	climateRangeStartBox->setPosition(472.f, 259.f);
	climateRangeStartBox->setDefaultText("0.0");
	climateRangeStartBox->setText("0.0");
	climateRangeStartBox->onTextChange([this]() {
		auto w = gui.get<tgui::EditBox>("climateRangeStartBox");
		this->F_manageNumInput(w, false, false);
	});


	tgui::EditBox::Ptr climateRangeStopArea = tgui::EditBox::create();
	registerHeader(climateRangeStopArea);
	climateRangeStopArea->setPosition(556.f, 242.f);
	climateRangeStopArea->setText("Range End");

	tgui::TextArea::Ptr climateRangeStopInfo = tgui::TextArea::create();
	climateRangeStopInfo->setSize(145.f, 54.f);
	climateRangeStopInfo->addText("Valid range is between 0 and 1.0. 0 (and lower) is extreme cold, while 1.0 (and higher) is extreme heat");
	climateRangeStopInfo->setTextSize(9u);
	climateRangeStopInfo->setReadOnly();
	climateRangeStopArea->setToolTip(climateRangeStopInfo);

	tgui::EditBox::Ptr climateRangeStopBox = tgui::EditBox::create();
	climateRangeStopBox->setSize(70.f, 25.f);
	climateRangeStopBox->setPosition(556.f, 259.f);
	climateRangeStopBox->setDefaultText("1.0");
	climateRangeStopBox->setText("1.0");
	climateRangeStopBox->onTextChange([this]() {
		auto w = gui.get<tgui::EditBox>("climateRangeStopBox");
		this->F_manageNumInput(w, false, false);
	});


	tgui::EditBox::Ptr climateMaxModifyArea = tgui::EditBox::create();
	registerHeader(climateMaxModifyArea);
	climateMaxModifyArea->setPosition(640.f, 242.f);
	climateMaxModifyArea->setText("Max Change");
	climateMaxModifyArea->setEnabled(false);

	tgui::TextArea::Ptr climateMaxModifyInfo = tgui::TextArea::create();
	climateMaxModifyInfo->setSize(145.f, 64.f);
	climateMaxModifyInfo->addText("The largest value added to the climate range in equator mode; the larger it is, the hotter the equator line will be");
	climateMaxModifyInfo->setTextSize(9u);
	climateMaxModifyInfo->setReadOnly();
	climateMaxModifyArea->setToolTip(climateMaxModifyInfo);

	tgui::EditBox::Ptr climateMaxModifyBox = tgui::EditBox::create();
	climateMaxModifyBox->setSize(70.f, 25.f);
	climateMaxModifyBox->setPosition(640.f, 259.f);
	climateMaxModifyBox->setDefaultText("0.0");
	climateMaxModifyBox->setText("0.0");
	climateMaxModifyBox->onTextChange([this]() {
		auto w = gui.get<tgui::EditBox>("climateMaxModifyBox");
		this->F_manageNumInput(w, false, true);
	});
	climateMaxModifyBox->setEnabled(false);

	//********************************************************************************************* MOISTURE GUI GROUP

	tgui::EditBox::Ptr moistureArea = tgui::EditBox::create(); //Distance of 35.f
	moistureArea->setReadOnly();
	moistureArea->setSize(50.f, 16.f);
	moistureArea->setPosition(375.f, 294.f);
	moistureArea->setText("Humidity");
	moistureArea->setTextSize(9u);

	tgui::TextArea::Ptr moistureInfo = tgui::TextArea::create();
	moistureInfo->setSize(145.f, 27.f);
	moistureInfo->addText("Moisture generation. Final piece of biome generation");
	moistureInfo->setTextSize(9u);
	moistureInfo->setReadOnly();
	moistureArea->setToolTip(moistureInfo);


	tgui::EditBox::Ptr moistureCruncherArea = tgui::EditBox::create(); //Distance of 21.f
	registerHeader(moistureCruncherArea);
	moistureCruncherArea->setPosition(388.f, 315.f);
	moistureCruncherArea->setText("Cruncher");

	tgui::TextArea::Ptr moistureCruncherInfo = tgui::TextArea::create();
	moistureCruncherInfo->setSize(145.f, 40.f);
	moistureCruncherInfo->addText("As the value increases, humidity becomes more \"extreme\"");
	moistureCruncherInfo->setTextSize(9u);
	moistureCruncherInfo->setReadOnly();
	moistureCruncherArea->setToolTip(moistureCruncherInfo);

	tgui::EditBox::Ptr moistureCruncherBox = tgui::EditBox::create(); //Distance of 17.f
	moistureCruncherBox->setSize(70.f, 25.f);
	moistureCruncherBox->setPosition(388.f, 332.f);
	moistureCruncherBox->setDefaultText("1.0");
	moistureCruncherBox->setText("1.0");
	moistureCruncherBox->onTextChange([this]() {
		auto w = gui.get<tgui::EditBox>("moistureCruncherBox");
		this->F_manageNumInput(w, false, true);
	});


	tgui::EditBox::Ptr beachPointArea = tgui::EditBox::create();
	registerHeader(beachPointArea);
	beachPointArea->setPosition(480.f, 315.f);
	beachPointArea->setText("Beach Point");

	tgui::TextArea::Ptr beachPointInfo = tgui::TextArea::create();
	beachPointInfo->setSize(145.f, 84.f);
	beachPointInfo->addText("Signifies the point to where land is made into a coastal biome. If the value is smaller than the land threshold, no beaches will be present. Range is between -1.0 and 1.0");
	beachPointInfo->setTextSize(9u);
	beachPointInfo->setReadOnly();
	beachPointArea->setToolTip(beachPointInfo);

	tgui::EditBox::Ptr beachPointBox = tgui::EditBox::create();
	beachPointBox->setSize(70.f, 25.f);
	beachPointBox->setPosition(480.f, 332.f);
	beachPointBox->setDefaultText("0.32");
	beachPointBox->setText("0.32");
	beachPointBox->onTextChange([this]() {
		auto w = gui.get<tgui::EditBox>("beachPointBox");
		this->F_manageNumInput(w, false, true);
	});

	//********************************************************************************************* DECORATORS

	tgui::Label::Ptr extraLabel = tgui::Label::create();
	extraLabel->setPosition(5.f, 342.f);
	extraLabel->setText("version: indev\nHold F4 to zoom. Hover over headers for more info");
	extraLabel->setTextSize(9u);

	//********************************************************************************************* GUI CONSTRUCTION

	gui.add(consoleBox, "consoleBox");
	gui.add(flushBtn, "flushBtn");

	gui.add(startBtn, "startBtn");
	gui.add(cancelBtn, "cancelBtn");
	gui.add(saveBtn, "saveBtn");

	gui.add(worldArea, "worldArea");
	gui.add(loopXArea, "loopXArea");
	gui.add(loopXBtn, "loopXBtn");
	gui.add(loopYArea, "loopYArea");
	gui.add(loopYBtn, "loopYBtn");
	gui.add(widthArea, "widthArea");
	gui.add(widthBox, "widthBox");
	gui.add(heightArea, "heightArea");
	gui.add(heightBox, "heightBox");
	gui.add(seedArea, "seedArea");
	gui.add(seedBox, "seedBox");

	gui.add(continentalArea, "continentalArea");
	gui.add(landCruncherArea, "landCruncherArea");
	gui.add(landCruncherBox, "landCruncherBox");
	gui.add(landPointArea, "landPointArea");
	gui.add(landPointBox, "landPointBox");
	gui.add(landSparsityArea, "landSparsityArea");
	gui.add(landSparsityBox, "landSparsityBox");

	gui.add(islandArea, "islandArea");
	gui.add(islandCruncherArea, "islandCruncherArea");
	gui.add(islandCruncherBox, "islandCruncherBox");
	gui.add(islandPointArea, "islandPointArea");
	gui.add(islandPointBox, "islandPointBox");
	gui.add(islandSparsityArea, "islandSparsityArea");
	gui.add(islandSparsityBox, "islandSparsityBox");
	gui.add(islandPunishmentArea, "islandPunishmentArea");
	gui.add(islandPunishmentBox, "islandPunishmentBox");

	gui.add(climateArea, "climateArea");
	gui.add(useEquatorArea, "useEquatorArea");
	gui.add(useEquatorBtn, "useEquatorBtn");
	gui.add(climateCruncherArea, "climateCruncherArea");
	gui.add(climateCruncherBox, "climateCruncherBox");
	gui.add(climateRangeStartArea, "climateRangeStartArea");
	gui.add(climateRangeStartBox, "climateRangeStartBox");
	gui.add(climateRangeStopArea, "climateRangeStopArea");
	gui.add(climateRangeStopBox, "climateRangeStopBox");
	gui.add(climateMaxModifyArea, "climateMaxModifyArea");
	gui.add(climateMaxModifyBox, "climateMaxModifyBox");

	gui.add(moistureArea, "moistureArea");
	gui.add(moistureCruncherArea, "moistureCruncherArea");
	gui.add(moistureCruncherBox, "moistureCruncherBox");
	gui.add(beachPointArea, "beachPointArea");
	gui.add(beachPointBox, "beachPointBox");

	gui.add(extraLabel, "extraLabel");
}


/*********************************************************************************************
Simplifies "header" or "area" creation - the text that hovers over an editable box
*********************************************************************************************/
void registerHeader(tgui::EditBox::Ptr header)
{
	header->setReadOnly();
	header->setSize(70.f, 16.f);
	header->setTextSize(9u);
	header->setAlignment(tgui::EditBox::Alignment::Center);
}