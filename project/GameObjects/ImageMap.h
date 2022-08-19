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

	ImageMap(unsigned int width, unsigned int height, bool formObj = true);
	bool reform(unsigned int width, unsigned int height); //Resizes the VertexArray

	bool loadMap(sf::Vector2u tileSize, float scale = 1.0, bool scuff = false);
	bool loadMap(sf::Vector2u tileSize, int R, int G, int B, int A, float scale = 1.0, bool scuff = false); //Loads a specific color for all spaces

	void fitToSpace(sf::Vector2f coordinates, sf::Vector2f lengths, bool scuff = false); //Reloads the map
	void fitToSpace(sf::Vector2f coordinates, sf::Vector2f lengths, int R, int G, int B, int A, bool scuff = false);

	bool updateTile(sf::Vector2u tileSpace, int R, int G, int B, int A); //Changes a tile's color without changing the map array

	unsigned int getWidth() const;
	unsigned int getHeight() const;
	float getTileWidth() const;
	float getTileHeight() const;
	bool isScuffed() const;

protected:

	unsigned int width; //In number of tiles
	unsigned int height;

	float tileWidth; //In pixels
	float tileHeight;

	bool scuffed; //"Scuffed" mode indicates whether the VertexArray is simply scaled, or completely recreated when scale() is called
	float scale = 0.f;

private:

	unsigned int originalWidth; //Keep track of the unscaled widths and heights that were entered
	unsigned int originalHeight;

	float originalTileWidth;
	float originalTileHeight;
};