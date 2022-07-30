#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include "TileMap.h"

TileMap::TileMap(unsigned int w, unsigned int h) {
	obj.setPrimitiveType(sf::Quads);
	bool failed = false;

	try
	{
		obj.resize(w * h * 4);
	}
	catch (std::length_error e)
	{
		std::cout << "Invalid size for VectorArray inside TileMap object" << std::endl;
		failed = true;
	}
	catch (std::bad_alloc e)
	{
		std::cout << "Could not allocate enough memory for a VectorArray inside TileMap object" << std::endl;
		failed = true;
	}

	if (!failed)
	{
		width = w;
		height = h;
	}
	else
	{
		width = 0u; //Failsafe for when SFML cannot create the TileMap
		height = 0u;
	}

	tileWidth = 0.f; //Later initialized when a map is loaded
	tileHeight = 0.f;
	texturePixelWidth = 0;
	texturePixelHeight = 0;
}

TileMap::~TileMap() {}


bool TileMap::loadMap(sf::Vector2u tileSize, float scale)
{
	return this->loadMap(tileSize, 0, 0, 0, 0, scale);
}

bool TileMap::loadMap(sf::Vector2u tileSize, int R, int G, int B, int A, float scale)
{
	tileWidth = tileSize.x * scale;
	tileHeight = tileSize.y * scale;

	texturePixelWidth = 0;
	texturePixelHeight = 0;

	for (unsigned int i = 0; i < height; i++) {
		for (unsigned int j = 0; j < width; j++) {

			sf::Vertex* tile = &obj[(j + (i * width)) * 4];

			tile[0].position = sf::Vector2f(j * tileWidth, i * tileHeight);
			tile[1].position = sf::Vector2f((j + 1) * tileWidth, i * tileHeight);
			tile[2].position = sf::Vector2f((j + 1) * tileWidth, (i + 1) * tileHeight);
			tile[3].position = sf::Vector2f(j * tileWidth, (i + 1) * tileHeight);

			tile[0].color = sf::Color(R, G, B, A);
			tile[1].color = sf::Color(R, G, B, A);
			tile[2].color = sf::Color(R, G, B, A);
			tile[3].color = sf::Color(R, G, B, A);
		}
	}

	return true;
}

bool TileMap::loadMap(sf::Vector2u tileSize, std::vector<int> map, float scale) {
	
	if (map.size() != (width * height)) { //Incompatible map size
		return false;
	}

	if (!text) { //Null pointer; texture unanitialized
		return false;
	}

	float xScaled = tileSize.x * scale;
	float yScaled = tileSize.y * scale;

	tileWidth = xScaled;
	tileHeight = yScaled;

	texturePixelWidth = tileSize.x;
	texturePixelHeight = tileSize.y;

	for (unsigned int i = 0; i < height; i++) {
		for (unsigned int j = 0; j < width; j++) {

			int value = map[j + (i * width)]; //Should start at 0
			sf::Vertex* tile = &obj[(j + (i * width)) * 4];

			int tX = value % (text->getSize().x / tileSize.x); //Works with all well defined tilesets
			int tY = value / (text->getSize().x / tileSize.x);

			tile[0].position = sf::Vector2f(j * xScaled, i * yScaled);
			tile[1].position = sf::Vector2f((j + 1) * xScaled, i * yScaled);
			tile[2].position = sf::Vector2f((j + 1) * xScaled, (i + 1) * yScaled);
			tile[3].position = sf::Vector2f(j * xScaled, (i + 1) * yScaled);

			tile[0].texCoords = sf::Vector2f(tX * tileSize.x, tY * tileSize.y);
			tile[1].texCoords = sf::Vector2f((tX + 1) * tileSize.x, tY * tileSize.y);
			tile[2].texCoords = sf::Vector2f((tX + 1) * tileSize.x, (tY + 1) * tileSize.y);
			tile[3].texCoords = sf::Vector2f(tX * tileSize.x, (tY + 1) * tileSize.y);
		}
	}

	mapArray = map;
	return true;
}



bool TileMap::updateTile(sf::Vector2u tileSpace, int newVal) {

	if (tileSpace.x > width || tileSpace.y > height) {
		return false;
	}

	if (!text) {
		return false;
	}

	if (texturePixelWidth == 0 || texturePixelHeight == 0) { //No map was loaded yet
		return false;
	}


	sf::Vertex* tile = &obj[(tileSpace.x + (tileSpace.y * width)) * 4];

	int tX = newVal % (text->getSize().x / texturePixelWidth);
	int tY = newVal / (text->getSize().x / texturePixelWidth);

	tile[0].texCoords = sf::Vector2f(tX * texturePixelWidth, tY * texturePixelHeight);
	tile[1].texCoords = sf::Vector2f((tX + 1) * texturePixelWidth, tY * texturePixelHeight);
	tile[2].texCoords = sf::Vector2f((tX + 1) * texturePixelWidth, (tY + 1) * texturePixelHeight);
	tile[3].texCoords = sf::Vector2f(tX * texturePixelWidth, (tY + 1) * texturePixelHeight);

	mapArray[tileSpace.x + (tileSpace.y * width)] = newVal;

	return true;
}

bool TileMap::updateTile(sf::Vector2u tileSpace, int R, int G, int B, int A) {

	if (tileSpace.x > width || tileSpace.y > height) {
		return false;
	}

	sf::Vertex* tile = &obj[(tileSpace.x + (tileSpace.y * width)) * 4];

	tile[0].color = sf::Color(R, G, B, A);
	tile[1].color = sf::Color(R, G, B, A);
	tile[2].color = sf::Color(R, G, B, A);
	tile[3].color = sf::Color(R, G, B, A);

	return true;
}

std::vector<int> TileMap::getMapArray() const { return mapArray; }
unsigned int TileMap::getWidth() const { return width; }
unsigned int TileMap::getHeight() const { return height; }