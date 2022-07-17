#include "WorldGenGUI.h"
#include <TGUI/TGUI.hpp>
#include <SFML/Graphics.hpp>

/*********************************************************************************************
Entirely responsible for the layout, styling, and functionality of the GUI upon loading
*********************************************************************************************/
void WorldGenGUI::initialize()
{
	mainFont = tgui::Font("consola.ttf");
	gui.setFont(mainFont);

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


	tgui::EditBox::Ptr worldArea = tgui::EditBox::create();
	worldArea->setReadOnly();
	worldArea->setSize(40.f, 16.f);
	worldArea->setPosition(375.f, 2.f);
	worldArea->setText("World");
	worldArea->setTextSize(9u);

	tgui::EditBox::Ptr widthArea = tgui::EditBox::create();
	widthArea->setReadOnly();
	widthArea->setSize(70.f, 16.f);
	widthArea->setPosition(388.f, 23.f);
	widthArea->setText("Map Width");
	widthArea->setTextSize(9u);
	widthArea->setAlignment(tgui::EditBox::Alignment::Center);

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
	heightArea->setReadOnly();
	heightArea->setSize(70.f, 16.f);
	heightArea->setPosition(480.f, 23.f);
	heightArea->setText("Map Height");
	heightArea->setTextSize(9u);
	heightArea->setAlignment(tgui::EditBox::Alignment::Center);

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
	seedArea->setReadOnly();
	seedArea->setSize(70.f, 16.f);
	seedArea->setPosition(572.f, 23.f);
	seedArea->setText("Seed");
	seedArea->setTextSize(9u);
	seedArea->setAlignment(tgui::EditBox::Alignment::Center);

	tgui::EditBox::Ptr seedBox = tgui::EditBox::create();
	seedBox->setSize(140.f, 25.f);
	seedBox->setPosition(572.f, 40.f);
	seedBox->setDefaultText("");
	seedBox->onTextChange([this]() {
		auto SB = gui.get<tgui::EditBox>("seedBox");
		this->F_scaleInput(SB, 19);
		});


	tgui::EditBox::Ptr continentalArea = tgui::EditBox::create(); //Distance of 35.f
	continentalArea->setReadOnly();
	continentalArea->setSize(70.f, 16.f);
	continentalArea->setPosition(375.f, 75.f);
	continentalArea->setText("Continental");
	continentalArea->setTextSize(9u);

	tgui::EditBox::Ptr landCruncherArea = tgui::EditBox::create(); //Distance of 21.f
	landCruncherArea->setReadOnly();
	landCruncherArea->setSize(70.f, 16.f);
	landCruncherArea->setPosition(388.f, 96.f);
	landCruncherArea->setText("Cruncher");
	landCruncherArea->setTextSize(9u);
	landCruncherArea->setAlignment(tgui::EditBox::Alignment::Center);
	//tgui::Label::Ptr lca = tgui::Label::create("As the value increases, continents become beefier");
	//landCruncherArea->setToolTip(lca);

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
	landPointArea->setReadOnly();
	landPointArea->setSize(70.f, 16.f);
	landPointArea->setPosition(480.f, 96.f);
	landPointArea->setText("Threshold");
	landPointArea->setTextSize(9u);
	landPointArea->setAlignment(tgui::EditBox::Alignment::Center);

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
	landSparsityArea->setReadOnly();
	landSparsityArea->setSize(70.f, 16.f);
	landSparsityArea->setPosition(572.f, 96.f);
	landSparsityArea->setText("Sparsity");
	landSparsityArea->setTextSize(9u);
	landSparsityArea->setAlignment(tgui::EditBox::Alignment::Center);

	tgui::EditBox::Ptr landSparsityBox = tgui::EditBox::create();
	landSparsityBox->setSize(70.f, 25.f);
	landSparsityBox->setPosition(572.f, 113.f);
	landSparsityBox->setDefaultText("350.0");
	landSparsityBox->setText("350.0");
	landSparsityBox->onTextChange([this]() {
		auto w = gui.get<tgui::EditBox>("landSparsityBox");
		this->F_manageNumInput(w, false, true);
		});


	tgui::EditBox::Ptr islandArea = tgui::EditBox::create(); //Distance of 35.f
	islandArea->setReadOnly();
	islandArea->setSize(50.f, 16.f);
	islandArea->setPosition(375.f, 148.f);
	islandArea->setText("Islands");
	islandArea->setTextSize(9u);

	tgui::EditBox::Ptr islandCruncherArea = tgui::EditBox::create(); //Distance of 21.f
	islandCruncherArea->setReadOnly();
	islandCruncherArea->setSize(70.f, 16.f);
	islandCruncherArea->setPosition(388.f, 169.f);
	islandCruncherArea->setText("Cruncher");
	islandCruncherArea->setTextSize(9u);
	islandCruncherArea->setAlignment(tgui::EditBox::Alignment::Center);

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
	islandPointArea->setReadOnly();
	islandPointArea->setSize(70.f, 16.f);
	islandPointArea->setPosition(472.f, 169.f);
	islandPointArea->setText("Spawn Area");
	islandPointArea->setTextSize(9u);
	islandPointArea->setAlignment(tgui::EditBox::Alignment::Center);

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
	islandSparsityArea->setReadOnly();
	islandSparsityArea->setSize(70.f, 16.f);
	islandSparsityArea->setPosition(556.f, 169.f);
	islandSparsityArea->setText("Sparsity");
	islandSparsityArea->setTextSize(9u);
	islandSparsityArea->setAlignment(tgui::EditBox::Alignment::Center);

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
	islandPunishmentArea->setReadOnly();
	islandPunishmentArea->setSize(70.f, 16.f);
	islandPunishmentArea->setPosition(640.f, 169.f);
	islandPunishmentArea->setText("Submergement");
	islandPunishmentArea->setTextSize(9u);
	islandPunishmentArea->setAlignment(tgui::EditBox::Alignment::Center);

	tgui::EditBox::Ptr islandPunishmentBox = tgui::EditBox::create();
	islandPunishmentBox->setSize(70.f, 25.f);
	islandPunishmentBox->setPosition(640.f, 186.f);
	islandPunishmentBox->setDefaultText("12.5");
	islandPunishmentBox->setText("12.5");
	islandPunishmentBox->onTextChange([this]() {
		auto w = gui.get<tgui::EditBox>("islandPunishmentBox");
		this->F_manageNumInput(w, false, false);
		});


	tgui::EditBox::Ptr climateArea = tgui::EditBox::create(); //Distance of 35.f
	climateArea->setReadOnly();
	climateArea->setSize(50.f, 16.f);
	climateArea->setPosition(375.f, 221.f);
	climateArea->setText("Climate");
	climateArea->setTextSize(9u);

	tgui::EditBox::Ptr useEquatorArea = tgui::EditBox::create();
	useEquatorArea->setReadOnly();
	useEquatorArea->setSize(70.f, 16.f);
	useEquatorArea->setPosition(620.f, 221.f);
	useEquatorArea->setText("Use Equator");
	useEquatorArea->setTextSize(9u);
	useEquatorArea->setAlignment(tgui::EditBox::Alignment::Center);

	tgui::ToggleButton::Ptr useEquatorBtn = tgui::ToggleButton::create();
	useEquatorBtn->setSize(16.f, 16.f);
	useEquatorBtn->setPosition(605.f, 221.f);

	tgui::EditBox::Ptr climateCruncherArea = tgui::EditBox::create(); //Distance of 21.f
	climateCruncherArea->setReadOnly();
	climateCruncherArea->setSize(70.f, 16.f);
	climateCruncherArea->setPosition(388.f, 242.f);
	climateCruncherArea->setText("Cruncher");
	climateCruncherArea->setTextSize(9u);
	climateCruncherArea->setAlignment(tgui::EditBox::Alignment::Center);

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
	climateRangeStartArea->setReadOnly();
	climateRangeStartArea->setSize(70.f, 16.f);
	climateRangeStartArea->setPosition(472.f, 242.f);
	climateRangeStartArea->setText("Range Start");
	climateRangeStartArea->setTextSize(9u);
	climateRangeStartArea->setAlignment(tgui::EditBox::Alignment::Center);

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
	climateRangeStopArea->setReadOnly();
	climateRangeStopArea->setSize(70.f, 16.f);
	climateRangeStopArea->setPosition(556.f, 242.f);
	climateRangeStopArea->setText("Range End");
	climateRangeStopArea->setTextSize(9u);
	climateRangeStopArea->setAlignment(tgui::EditBox::Alignment::Center);

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
	climateMaxModifyArea->setReadOnly();
	climateMaxModifyArea->setSize(70.f, 16.f);
	climateMaxModifyArea->setPosition(640.f, 242.f);
	climateMaxModifyArea->setText("Max Change");
	climateMaxModifyArea->setTextSize(9u);
	climateMaxModifyArea->setAlignment(tgui::EditBox::Alignment::Center);

	tgui::EditBox::Ptr climateMaxModifyBox = tgui::EditBox::create();
	climateMaxModifyBox->setSize(70.f, 25.f);
	climateMaxModifyBox->setPosition(640.f, 259.f);
	climateMaxModifyBox->setDefaultText("0.0");
	climateMaxModifyBox->setText("0.0");
	climateMaxModifyBox->onTextChange([this]() {
		auto w = gui.get<tgui::EditBox>("climateMaxModifyBox");
		this->F_manageNumInput(w, false, true);
		});


	tgui::EditBox::Ptr moistureArea = tgui::EditBox::create(); //Distance of 35.f
	moistureArea->setReadOnly();
	moistureArea->setSize(50.f, 16.f);
	moistureArea->setPosition(375.f, 294.f);
	moistureArea->setText("Humidity");
	moistureArea->setTextSize(9u);

	tgui::EditBox::Ptr moistureCruncherArea = tgui::EditBox::create(); //Distance of 21.f
	moistureCruncherArea->setReadOnly();
	moistureCruncherArea->setSize(70.f, 16.f);
	moistureCruncherArea->setPosition(388.f, 315.f);
	moistureCruncherArea->setText("Cruncher");
	moistureCruncherArea->setTextSize(9u);
	moistureCruncherArea->setAlignment(tgui::EditBox::Alignment::Center);

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
	beachPointArea->setReadOnly();
	beachPointArea->setSize(70.f, 16.f);
	beachPointArea->setPosition(480.f, 315.f);
	beachPointArea->setText("Beach Point");
	beachPointArea->setTextSize(9u);
	beachPointArea->setAlignment(tgui::EditBox::Alignment::Center);

	tgui::EditBox::Ptr beachPointBox = tgui::EditBox::create();
	beachPointBox->setSize(70.f, 25.f);
	beachPointBox->setPosition(480.f, 332.f);
	beachPointBox->setDefaultText("0.32");
	beachPointBox->setText("0.32");
	beachPointBox->onTextChange([this]() {
		auto w = gui.get<tgui::EditBox>("beachPointBox");
		this->F_manageNumInput(w, false, true);
		});


	gui.add(startBtn, "startBtn");
	gui.add(cancelBtn, "cancelBtn");

	gui.add(worldArea, "worldArea");
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
}