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

ImageMap::~ImageMap() {}


bool ImageMap::loadMap(sf::Vector2u tileSize, float scale)
{
	return this->loadMap(tileSize, 0, 0, 0, 0, scale);
}

bool ImageMap::loadMap(sf::Vector2u tileSize, int R, int G, int B, int A, float scale)
{
	tileWidth = tileSize.x * scale;
	tileHeight = tileSize.y * scale;

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



void ImageMap::fitToSpace(sf::Vector2f coordinates, sf::Vector2f lengths)
{
	this->fitToSpace(coordinates, lengths, 0, 0, 0, 255);
}

void ImageMap::fitToSpace(sf::Vector2f coordinates, sf::Vector2f lengths, int R, int G, int B, int A)
{
	float idealFactor = lengths.x / (float)lengths.y;
	float tileFactor = width / (float)height;

	if (tileFactor == idealFactor) //Fits perfectly
	{
		float s = lengths.x / (float)width;
		this->loadMap(sf::Vector2u(1u, 1u), R, G, B, A, s);
		this->setPosition(coordinates);
	}
	else if (tileFactor < idealFactor) //Width is too small; fit to height
	{
		float s = lengths.y / (float)height;
		float offset = ((float)lengths.x - (width * s)) / 2.0; //Center by width
		this->loadMap(sf::Vector2u(1u, 1u), R, G, B, A, s);
		this->setPosition(sf::Vector2f(coordinates.x + offset, coordinates.y));
	}
	else //Height is too small; fit to width
	{
		float s = lengths.x / (float)width;
		float offset = ((float)lengths.y - (height * s)) / 2.0; //Center by width
		this->loadMap(sf::Vector2u(1u, 1u), R, G, B, A, s);
		this->setPosition(sf::Vector2f(coordinates.x, coordinates.y + offset));
	}
}



bool ImageMap::updateTile(sf::Vector2u tileSpace, int R, int G, int B, int A) {

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

unsigned int ImageMap::getWidth() const { return width; }
unsigned int ImageMap::getHeight() const { return height; }