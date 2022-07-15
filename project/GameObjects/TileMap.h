#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include "Entity.h"
#include <vector>

/**
A basic map object, mapping a texture onto each tile, which are all represented by individual quads
*/
class TileMap : public Entity
{
public:

	TileMap(unsigned int width, unsigned int height);
	virtual ~TileMap();

	bool loadMap(sf::Vector2u tileSize, std::vector<int> map, float scale = 1.0);
	bool loadMap(sf::Vector2u tileSize, float scale = 1.0); //Loads no array
	bool loadMap(sf::Vector2u tileSize, int R, int G, int B, int A, float scale = 1.0); //Loads a specific color for all spaces
	bool updateTile(sf::Vector2u tileSpace, int newVal); //Changes a value at an index in the map array, as well as the map itself
	bool updateTile(sf::Vector2u tileSpace, int R, int G, int B, int A); //Changes a tile's color without changing the map array
	std::vector<int> getMapArray() const;

	unsigned int getWidth() const;
	unsigned int getHeight() const;

protected:

	unsigned int width; //In number of tiles
	unsigned int height;

	float tileWidth; //In pixels
	float tileHeight;

	unsigned int texturePixelWidth;
	unsigned int texturePixelHeight;

	std::vector<int> mapArray; //Stores information about the map loaded
};