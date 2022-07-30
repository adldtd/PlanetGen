#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include "Entity.h"
#include <vector>

/**
A basic map object, mapping a color onto each tile, which are all represented by individual quads
*/
class ImageMap : public Entity
{
public:

	ImageMap(unsigned int width, unsigned int height);
	virtual ~ImageMap();

	bool loadMap(sf::Vector2u tileSize, float scale = 1.0); //Loads no array
	bool loadMap(sf::Vector2u tileSize, int R, int G, int B, int A, float scale = 1.0); //Loads a specific color for all spaces

	void fitToSpace(sf::Vector2f coordinates, sf::Vector2f lengths); //Reloads the map
	void fitToSpace(sf::Vector2f coordinates, sf::Vector2f lengths, int R, int G, int B, int A);

	bool updateTile(sf::Vector2u tileSpace, int R, int G, int B, int A); //Changes a tile's color without changing the map array

	unsigned int getWidth() const;
	unsigned int getHeight() const;

protected:

	unsigned int width; //In number of tiles
	unsigned int height;

	float tileWidth; //In pixels
	float tileHeight;
};