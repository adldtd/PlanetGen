#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include "ImageMap.h"

ImageMap::ImageMap(unsigned int w, unsigned int h)
{
	obj.setPrimitiveType(sf::Quads);
	bool failed = false;

	try
	{
		obj.resize(w * h * 4);
	}
	catch (std::length_error& e)
	{
		std::cout << "Invalid size for VectorArray inside ImageMap object: " << e.what() << std::endl;
		failed = true;
	}
	catch (std::bad_alloc& e)
	{
		std::cout << "Could not allocate enough memory for a VectorArray inside ImageMap object: " << e.what() << std::endl;
		failed = true;
	}

	if (!failed)
	{
		width = w;
		height = h;
	}
	else
	{
		width = 0u; //Failsafe for when SFML cannot create the ImageMap
		height = 0u;
	}

	tileWidth = 0.f; //Later initialized when a map is loaded
	tileHeight = 0.f;
}



bool ImageMap::loadMap(sf::Vector2u tileSize, float scale, bool scuff)
{
	return this->loadMap(tileSize, 0, 0, 0, 0, scale, scuff);
}

bool ImageMap::loadMap(sf::Vector2u tileSize, int R, int G, int B, int A, float s, bool scuff)
{
	if (!scuff)
	{
		tileWidth = tileSize.x * s;
		tileHeight = tileSize.y * s;
	}
	else
	{
		float newWidth = width * s;
		float newHeight = height * s;
		width = floor(newWidth);
		height = floor(newHeight);

		float widthLost = (newWidth / (float)width);
		float heightLost = (newHeight / (float)height);
		tileWidth = tileSize.x * widthLost;
		tileHeight = tileSize.y * heightLost;
	}

	this->scale = s; //NO RELATION TO "getScale()" AND "setScale()" FUNCTIONS
	//std::cout << s << std::endl;

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

	scuffed = scuff; //Reset whenever a new map is loaded in
	return true;
}



void ImageMap::fitToSpace(sf::Vector2f coordinates, sf::Vector2f lengths, bool scuff)
{
	this->fitToSpace(coordinates, lengths, 0, 0, 0, 255, scuff);
}

void ImageMap::fitToSpace(sf::Vector2f coordinates, sf::Vector2f lengths, int R, int G, int B, int A, bool scuff)
{
	float idealFactor = lengths.x / (float)lengths.y;
	float tileFactor = width / (float)height;

	if (tileFactor == idealFactor) //Fits perfectly
	{
		float s = lengths.x / (float)width;
		if (scuff && s < 1.f) //Means that, even if scuff is specified, the map will not always be "scuffed"
			this->loadMap(sf::Vector2u(1u, 1u), R, G, B, A, s, true);
		else
			this->loadMap(sf::Vector2u(1u, 1u), R, G, B, A, s);
		this->setPosition(coordinates);
	}
	else if (tileFactor < idealFactor) //Width is too small; fit to height
	{
		float s = lengths.y / (float)height;
		float offset = ((float)lengths.x - (width * s)) / 2.0; //Center by width
		if (scuff && s < 1.f)
			this->loadMap(sf::Vector2u(1u, 1u), R, G, B, A, s, true);
		else
			this->loadMap(sf::Vector2u(1u, 1u), R, G, B, A, s);
		this->setPosition(sf::Vector2f(coordinates.x + offset, coordinates.y));
	}
	else //Height is too small; fit to width
	{
		float s = lengths.x / (float)width;
		float offset = ((float)lengths.y - (height * s)) / 2.0; //Center by width
		if (scuff && s < 1.f) //Means that, even if scuff is specified, the map will not always be "scuffed"
			this->loadMap(sf::Vector2u(1u, 1u), R, G, B, A, s, true);
		else
			this->loadMap(sf::Vector2u(1u, 1u), R, G, B, A, s);
		this->setPosition(sf::Vector2f(coordinates.x, coordinates.y + offset));
	}
}



bool ImageMap::updateTile(sf::Vector2u tileSpace, int R, int G, int B, int A) {

	unsigned int x;
	unsigned int y;

	if (scuffed)
	{
		x = floor(tileSpace.x * scale);
		y = floor(tileSpace.y * scale);
	}
	else
	{
		x = tileSpace.x;
		y = tileSpace.y;
	}

	if (x > width || y > height) {
		return false;
	}

	sf::Vertex* tile = &obj[(x + (y * width)) * 4];

	tile[0].color = sf::Color(R, G, B, A);
	tile[1].color = sf::Color(R, G, B, A);
	tile[2].color = sf::Color(R, G, B, A);
	tile[3].color = sf::Color(R, G, B, A);

	return true;
}

unsigned int ImageMap::getWidth() const { return width; }
unsigned int ImageMap::getHeight() const { return height; }
bool ImageMap::isScuffed() const { return scuffed; }