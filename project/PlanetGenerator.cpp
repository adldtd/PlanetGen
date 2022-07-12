#include <iostream>
#include <ios>
#include <cmath>
#include <fstream>
#include <chrono>
#include "PerlinNoise.hpp"
#include "PlanetGenerator.h"


/*
* BIOMES:
* 
*	Desert
*	Arid Grasslands
*	Grasslands
*	Savanna
*	Plains
*	Monsoon-Plains
*	Monsoon
*	Seasonal Rainforest
*	Evergreen Rainforest
*	Muddy Forest
*	Muddy Swamp
*	Grassy Plains
*	Deciduous Forest Plains
*	Deciduous Forest
*	Swamp
*	Arid Plains
*	Marshy Grasslands
*	Grassy Deciduous Forest Plains
*	Tall Grasslands
*	Rocky Plains
*	Coniferous Forest Plains
*	Mixed Forest Plains
*	Mixed Forest
*	Muddy Grasslands Coniferous Forest
*	Rocky Coniferous Forest Plains
*	Taiga
*	Coniferous Forest
*	Muddy Coniferous Forest
*	Rocky Snowy Plains
*	Snowy Taiga
*	Coniferous Swamp
*	Ice Sheet
*	Rocky Tundra
*	Tundra
*/


void generateEarth(unsigned int lengthX, unsigned int lengthY, std::string fileLocation) {

	//auto t = std::chrono::system_clock::now();
	//srand(t.time_since_epoch().count());
	srand(500u);

	bool polarX = true;
	bool polarY = true;

	siv::PerlinNoise::seed_type continentSeed = rand();
	siv::PerlinNoise::seed_type islandSeed = rand();
	siv::PerlinNoise::seed_type moistureSeed = rand();

	siv::PerlinNoise cgen{ continentSeed };
	siv::PerlinNoise igen{ islandSeed };
	siv::PerlinNoise mgen{ moistureSeed };

	std::ofstream earthFile;
	earthFile.open(fileLocation, std::ios::out | std::ios::binary);
	earthFile.seekp(0);

	unsigned int length = lengthX * lengthY;

	char* buffer = new char[length];
	double* elevation = new double[length]; //Records the elevation of each step
	double* moisture = new double[length];
	double* climate = new double[length]; //Determines the climate type of each step

	//Continent variables
	double LANDPOINT = 0.30; //Larger means more land
	double SPARSITY = 350.0; //This value works best at lengths 1472u 736u; larger means more space between land and sea
	double ISPARSITY = SPARSITY / 7.0; //Sparsity, but for islands
	double IPOINT = -0.01; //The point at which an island is most likely to spawn; farther from landpoint means farther from land
	double IPUNISH = 12.5; //Larger means the island will become more submerged the farther it gets from IPOINT

	//Climate variables
	bool USINGEQUATOR = true;
	double CLIMATECRUNCHER = 1.0; //Larger means the climate will be more "extreme"
	double RANGESTART = 0; //The wider the range of these two variables, the more "smooth" equator heat generation will be
	double RANGEEND = 0.45;
	double MAXMODIFY = 1 - RANGEEND; //The largest value that will be added to the range; the larger it is the more "hotter" the equator will be

	//Biome variables
	double BEACHPOINT = 0.33; //Larger means more and more land will be a beach biome

	const double TWOPI = 6.2832;
	double angleRadiusX = (1.0 / SPARSITY) / (TWOPI / double(lengthX));
	double angleRadiusY = (1.0 / SPARSITY) / (TWOPI / double(lengthY));
	double iAngleRadiusX = (1.0 / ISPARSITY) / (TWOPI / double(lengthX));
	double iAngleRadiusY = (1.0 / ISPARSITY) / (TWOPI / double(lengthY));

	double equator = (lengthY - 1.0) / 2.0; //The vertical line of the map
	double modifier = 3.00; //Makes sure the edges of the map are always water for looping
	double limit = 0.50;
	double borderX = (lengthX - 1.0) * limit;
	double borderY = (lengthY - 1.0) * limit;



	std::cout << "Generating terrain" << std::endl;
	for (int r = 0; r < lengthY; r++) { //Continent + island generation

		double angleY = 0.0;
		if (polarY)
			angleY = (r / double(lengthY)) * TWOPI;

		for (int c = 0; c < lengthX; c++) {

			double angleX = 0.0;
			if (polarX)
				angleX = (c / double(lengthX)) * TWOPI;

			double result = 0.0;
			if (polarX && polarY) {
				double pX1 = cos(angleX); double pY1 = sin(angleX);
				double pX2 = cos(angleY); double pY2 = sin(angleY);
				result = cgen.noise4D(pX1 * angleRadiusX, pY1 * angleRadiusX, pX2 * angleRadiusY, pY2 * angleRadiusY) + cgen.octave4D(pX1 * angleRadiusX, pY1 * angleRadiusX, pX2 * angleRadiusY, pY2 * angleRadiusY, 10);
				result *= 1.0;
			}
			else if (polarX) {
				double pX = cos(angleX); double pY = sin(angleX);
				result = cgen.noise3D(pX * angleRadiusX, pY * angleRadiusX, r / SPARSITY) + cgen.octave3D(pX * angleRadiusX, pY * angleRadiusX, r / SPARSITY, 10);
				result *= 1.0; //Scaled
			}
			else if (polarY) {
				double pX = cos(angleY); double pY = sin(angleY);
				result = cgen.noise3D(pX * angleRadiusY, pY * angleRadiusY, c / SPARSITY) + cgen.octave3D(pX * angleRadiusY, pY * angleRadiusY, c / SPARSITY, 10);
				result *= 1.0;
			}
			else //No looping
				result = cgen.noise2D(c / SPARSITY, r / SPARSITY) + cgen.octave2D(c / SPARSITY, r / SPARSITY, 10);


			int index = c + (r * lengthX);

			if (result < LANDPOINT - 0.08) {

				double shoreFactor = IPUNISH * std::abs((IPOINT - result));
				if (polarX && polarY) {
					double pX1 = cos(angleX); double pY1 = sin(angleX);
					double pX2 = cos(angleY); double pY2 = sin(angleY);
					result = igen.noise4D(pX1 * iAngleRadiusX, pY1 * iAngleRadiusX, pX2 * iAngleRadiusY, pY2 * iAngleRadiusY) + igen.octave4D(pX1 * iAngleRadiusX, pY1 * iAngleRadiusX, pX2 * iAngleRadiusY, pY2 * iAngleRadiusY, 4);
					result *= 1.0;
				}
				else if (polarX) {
					double pX = cos(angleX); double pY = sin(angleX);
					result = igen.noise3D(pX * iAngleRadiusX, pY * iAngleRadiusX, r / ISPARSITY) + igen.octave3D(pX * iAngleRadiusX, pY * iAngleRadiusX, r / ISPARSITY, 4);
					result *= 1.0;
				}
				else if (polarY) {
					double pX = cos(angleY); double pY = sin(angleY);
					result = igen.noise3D(pX * iAngleRadiusY, pY * iAngleRadiusY, c / ISPARSITY) + igen.octave3D(pX * iAngleRadiusY, pY * iAngleRadiusY, c / ISPARSITY, 4);
					result *= 1.0;
				}
				else
					result = igen.noise2D(c / ISPARSITY, r / ISPARSITY) + igen.octave2D(c / ISPARSITY, r / ISPARSITY, 4);
				result -= shoreFactor;
				result = result / 4.0; //Scaling

				if (result < LANDPOINT - 0.08)
					buffer[index] = 0;
				else if (result < LANDPOINT - 0.03)
					buffer[index] = 1;
				else if (result < LANDPOINT)
					buffer[index] = 2;
				else
					buffer[index] = 3;

			}
			else if (result < LANDPOINT - 0.03) {
				buffer[index] = 1; //Shallowish water
			}
			else if (result < LANDPOINT) {
				buffer[index] = 2; //Shallow water
			}
			else
				buffer[index] = 3; //Land

			elevation[index] = result;
		}
	}


	
	std::cout << "Generating biomes" << std::endl;
	siv::PerlinNoise::seed_type climateSeed = rand();
	siv::PerlinNoise clgen{ climateSeed };
	//double factor = 1.0 / normalFunction(equator, equator, 100.0); //Range is from 1 to 0

	for (int r = 0; r < lengthY; r++) { //Climate mapping

		double angleY = 0.0;
		if (polarY)
			angleY = (r / double(lengthY)) * TWOPI;

		//double heatValue = normalFunction(r, equator, 100.0) * factor;
		//double heatValue = 1.0 - (abs(r - equator) / equator);

		double modify = 0.0;
		if (USINGEQUATOR) {
			if (r < equator)
				modify = MAXMODIFY * (r / equator);
			else
				modify = MAXMODIFY * ((equator - (r - equator)) / equator);
		}

		for (int c = 0; c < lengthX; c++) {

			double angleX = 0.0;
			if (polarX)
				angleX = (c / double(lengthX)) * TWOPI;

			double heatValue = 0.0;
			if (polarX && polarY) {
				double pX1 = cos(angleX); double pY1 = sin(angleX);
				double pX2 = cos(angleY); double pY2 = sin(angleY);
				heatValue = clgen.noise4D(pX1 * angleRadiusX, pY1 * angleRadiusX, pX2 * angleRadiusY, pY2 * angleRadiusY) + clgen.octave4D(pX1 * angleRadiusX, pY1 * angleRadiusX, pX2 * angleRadiusY, pY2 * angleRadiusY, 6);
				heatValue *= CLIMATECRUNCHER;
			}
			else if (polarX) {
				double pX = cos(angleX); double pY = sin(angleX);
				heatValue = clgen.noise3D(pX * angleRadiusX, pY * angleRadiusX, r / SPARSITY) + clgen.octave3D(pX * angleRadiusX, pY * angleRadiusX, r / SPARSITY, 6);
				heatValue *= CLIMATECRUNCHER;
			}
			else if (polarY) {
				double pX = cos(angleY); double pY = sin(angleY);
				heatValue = clgen.noise3D(pX * angleRadiusY, pY * angleRadiusY, c / SPARSITY) + clgen.octave3D(pX * angleRadiusY, pY * angleRadiusY, c / SPARSITY, 6);
				heatValue *= CLIMATECRUNCHER;
			}
			else
				heatValue = clgen.noise2D(c / SPARSITY, r / SPARSITY) + clgen.octave2D(c / SPARSITY, r / SPARSITY, 6);
			
			if (USINGEQUATOR)
				heatValue = normalizeToRange(-1, 1, RANGESTART + modify, RANGEEND + modify, heatValue);
			else
				heatValue = (heatValue * 0.5) + 0.5; //Normalize to range 0, 1

			//double heatModifier = clgen.noise2D(c / 550.0, r / 550.0) + clgen.octave2D(c / 550.0, r / 550.0, 3); //From about -1.52 to 1.52
			//heatValue += (heatModifier * 0.131579 * 0.2);
			int index = c + (r * lengthX);
			climate[index] = heatValue;
		}
	}



	for (int r = 0; r < lengthY; r++) { //Biome generation

		double angleY = 0.0;
		if (polarY)
			angleY = (r / double(lengthY)) * TWOPI;

		for (int c = 0; c < lengthX; c++) {

			double angleX = 0.0;
			if (polarX)
				angleX = (c / double(lengthX)) * TWOPI;

			int index = c + (r * lengthX);

			double result = 0.0;
			if (polarX && polarY) { //TODO; needs 4D Perlin noise
				double pX1 = cos(angleX); double pY1 = sin(angleX);
				double pX2 = cos(angleY); double pY2 = sin(angleY);
				result = mgen.noise4D(pX1 * angleRadiusX, pY1 * angleRadiusX, pX2 * angleRadiusY, pY2 * angleRadiusY) + mgen.octave4D(pX1 * angleRadiusX, pY1 * angleRadiusX, pX2 * angleRadiusY, pY2 * angleRadiusY, 10);
				result *= 0.6826;
			}
			else if (polarX) {
				double pX = cos(angleX); double pY = sin(angleX);
				result = mgen.noise3D(pX * angleRadiusX, pY * angleRadiusX, r / SPARSITY) + mgen.octave3D(pX * angleRadiusX, pY * angleRadiusX, r / SPARSITY, 10);
				result *= 0.6826; //Scaled
			}
			else if (polarY) {
				double pX = cos(angleY); double pY = sin(angleY);
				result = mgen.noise3D(pX * angleRadiusY, pY * angleRadiusY, c / SPARSITY) + mgen.octave3D(pX * angleRadiusY, pY * angleRadiusY, c / SPARSITY, 10);
				result *= 0.6826;
			}
			else //No looping
				result = mgen.noise2D(c / SPARSITY, r / SPARSITY) + mgen.octave2D(c / SPARSITY, r / SPARSITY, 10);
			result *= 0.955414;

			if (buffer[index] == 3) { //Land

				if (result < -1.17) { //******************************************************************Arid

					if (climate[index] < 0.11) { //Cold
						buffer[index] = genIceSheet(r, c);
					}
					else if (climate[index] < 0.22) { //Cold to chilly
						if (elevation[index] < BEACHPOINT)
							buffer[index] = genRockyCliff(r, c);
						else
							buffer[index] = genRockySnowyPlains(r, c);
					}
					else if (climate[index] < 0.33) { //Chilly
						if (elevation[index] < BEACHPOINT)
							buffer[index] = genRockyCliff(r, c);
						else
							buffer[index] = genRockyPlains(r, c);
					}
					else if (climate[index] < 0.44) { //Chilly to cool
						if (elevation[index] < BEACHPOINT)
							buffer[index] = genRockyBeach(r, c);
						else
							buffer[index] = genRockyPlains(r, c);
					}
					else if (climate[index] < 0.55) { //Cool
						if (elevation[index] < BEACHPOINT)
							buffer[index] = genBeach(r, c);
						else
							buffer[index] = genPlains(r, c);
					}
					else if (climate[index] < 0.66) { //Cool to warm
						if (elevation[index] < BEACHPOINT)
							buffer[index] = genBeach(r, c);
						else
							buffer[index] = genAridPlains(r, c);
					}
					else if (climate[index] < 0.77) { //Warm
						buffer[index] = genDesert(r, c);
					}
					else if (climate[index] < 0.88) { //Warm to hot
						buffer[index] = genDesert(r, c);
					}
					else { //Hot
						buffer[index] = genDesert(r, c);
					}

				}
				else if (result < -0.84) { //*************************************************************Arid to dry

					if (climate[index] < 0.2) { //Cold
						if (elevation[index] < BEACHPOINT)
							buffer[index] = genRockyCliff(r, c);
						else
							buffer[index] = genRockyTundra(r, c);
					}
					else if (climate[index] < 0.4) { //Chilly
						if (elevation[index] < BEACHPOINT)
							buffer[index] = genRockyBeach(r, c);
						else
							buffer[index] = genRockyConiferousForestPlains(r, c);
					}
					else if (climate[index] < 0.6) { //Cool
						if (elevation[index] < BEACHPOINT)
							buffer[index] = genBeach(r, c);
						else
							buffer[index] = genPlains(r, c);
					}
					else if (climate[index] < 0.8) { //Warm
						if (elevation[index] < BEACHPOINT)
							buffer[index] = genBeach(r, c);
						else
							buffer[index] = genAridGrasslands(r, c);
					}
					else { //Hot
						if (elevation[index] < BEACHPOINT)
							buffer[index] = genBeach(r, c);
						else
							buffer[index] = genAridGrasslands(r, c);
					}

				}
				else if (result < -0.51) { //*************************************************************Dry

					if (climate[index] < 0.11) { //Cold
						if (elevation[index] < BEACHPOINT)
							buffer[index] = genRockyCliff(r, c);
						else
							buffer[index] = genTundra(r, c);
					}
					else if (climate[index] < 0.22) { //Cold to chilly
						if (elevation[index] < BEACHPOINT)
							buffer[index] = genRockyBeach(r, c);
						else
							buffer[index] = genSnowyTaiga(r, c);
					}
					else if (climate[index] < 0.33) { //Chilly
						if (elevation[index] < BEACHPOINT)
							buffer[index] = genBeach(r, c);
						else
							buffer[index] = genTaiga(r, c);
					}
					else if (climate[index] < 0.44) { //Chilly to cool
						if (elevation[index] < BEACHPOINT)
							buffer[index] = genBeach(r, c);
						else
							buffer[index] = genConiferousForestPlains(r, c);
					}
					else if (climate[index] < 0.55) { //Cool
						if (elevation[index] < BEACHPOINT)
							buffer[index] = genBeach(r, c);
						else
							buffer[index] = genPlains(r, c);
					}
					else if (climate[index] < 0.66) { //Cool to warm
						if (elevation[index] < BEACHPOINT)
							buffer[index] = genBeach(r, c);
						else
							buffer[index] = genGrassyPlains(r, c);
					}
					else if (climate[index] < 0.77) { //Warm
						if (elevation[index] < BEACHPOINT)
							buffer[index] = genBeach(r, c);
						else
							buffer[index] = genGrasslands(r, c);
					}
					else if (climate[index] < 0.88) { //Warm to hot
						if (elevation[index] < BEACHPOINT)
							buffer[index] = genBeach(r, c);
						else
							buffer[index] = genGrasslands(r, c);
					}
					else { //Hot
						if (elevation[index] < BEACHPOINT)
							buffer[index] = genBeach(r, c);
						else
							buffer[index] = genGrasslands(r, c);
					}

				}
				else if (result < -0.18) { //*************************************************************Dry to normal

					if (climate[index] < 0.2) { //Cold
						if (elevation[index] < BEACHPOINT)
							buffer[index] = genRockyBeach(r, c);
						else
							buffer[index] = genSnowyTaiga(r, c);
					}
					else if (climate[index] < 0.4) { //Chilly
						if (elevation[index] < BEACHPOINT)
							buffer[index] = genBeach(r, c);
						else
							buffer[index] = genConiferousForest(r, c);
					}
					else if (climate[index] < 0.6) { //Cool
						if (elevation[index] < BEACHPOINT)
							buffer[index] = genBeach(r, c);
						else
							buffer[index] = genDeciduousForestPlains(r, c);
					}
					else if (climate[index] < 0.8) { //Warm
						if (elevation[index] < BEACHPOINT)
							buffer[index] = genBeach(r, c);
						else
							buffer[index] = genGrassyPlains(r, c);
					}
					else { //Hot
						if (elevation[index] < BEACHPOINT)
							buffer[index] = genBeach(r, c);
						else
							buffer[index] = genSavanna(r, c);
					}

				}
				else if (result < 0.15) { //*************************************************************Normal

					if (climate[index] < 0.11) { //Cold
						if (elevation[index] < BEACHPOINT)
							buffer[index] = genBeach(r, c);
						else
							buffer[index] = genTaiga(r, c);
					}
					else if (climate[index] < 0.22) { //Cold to chilly
						if (elevation[index] < BEACHPOINT)
							buffer[index] = genBeach(r, c);
						else
							buffer[index] = genConiferousForest(r, c);
					}
					else if (climate[index] < 0.33) { //Chilly
						if (elevation[index] < BEACHPOINT)
							buffer[index] = genBeach(r, c);
						else
							buffer[index] = genConiferousForest(r, c);
					}
					else if (climate[index] < 0.44) { //Chilly to cool
						if (elevation[index] < BEACHPOINT)
							buffer[index] = genBeach(r, c);
						else
							buffer[index] = genMixedForestPlains(r, c);
					}
					else if (climate[index] < 0.55) { //Cool
						if (elevation[index] < BEACHPOINT)
							buffer[index] = genBeach(r, c);
						else
							buffer[index] = genDeciduousForestPlains(r, c);
					}
					else if (climate[index] < 0.66) { //Cool to warm
						if (elevation[index] < BEACHPOINT)
							buffer[index] = genBeach(r, c);
						else
							buffer[index] = genDeciduousForestPlains(r, c);
					}
					else if (climate[index] < 0.77) { //Warm
						if (elevation[index] < BEACHPOINT)
							buffer[index] = genBeach(r, c);
						else
							buffer[index] = genPlains(r, c);
					}
					else if (climate[index] < 0.88) { //Warm to hot
						if (elevation[index] < BEACHPOINT)
							buffer[index] = genBeach(r, c);
						else
							buffer[index] = genPlains(r, c);
					}
					else { //Hot
						if (elevation[index] < BEACHPOINT)
							buffer[index] = genBeach(r, c);
						else
							buffer[index] = genPlains(r, c);
					}

				}
				else if (result < 0.48) { //*************************************************************Normal to watery

					if (climate[index] < 0.2) { //Cold
						if (elevation[index] < BEACHPOINT)
							buffer[index] = genBeach(r, c);
						else
							buffer[index] = genConiferousForest(r, c);
					}
					else if (climate[index] < 0.4) { //Chilly
						if (elevation[index] < BEACHPOINT)
							buffer[index] = genBeach(r, c);
						else
							buffer[index] = genConiferousForest(r, c);
					}
					else if (climate[index] < 0.6) { //Cool
						if (elevation[index] < BEACHPOINT)
							buffer[index] = genBeach(r, c);
						else
							buffer[index] = genDeciduousForest(r, c);
					}
					else if (climate[index] < 0.8) { //Warm
						if (elevation[index] < BEACHPOINT)
							buffer[index] = genBeach(r, c);
						else
							buffer[index] = genDeciduousForestPlains(r, c);
					}
					else { //Hot
						if (elevation[index] < BEACHPOINT)
							buffer[index] = genBeach(r, c);
						else
							buffer[index] = genMonsoonPlains(r, c);
					}

				}
				else if (result < 0.81) { //*************************************************************Watery

					if (climate[index] < 0.11) { //Cold
						if (elevation[index] < BEACHPOINT)
							buffer[index] = genBeach(r, c);
						else
							buffer[index] = genConiferousForest(r, c);
					}
					else if (climate[index] < 0.22) { //Cold to chilly
						if (elevation[index] < BEACHPOINT)
							buffer[index] = genBeach(r, c);
						else
							buffer[index] = genConiferousForest(r, c);
					}
					else if (climate[index] < 0.33) { //Chilly
						if (elevation[index] < BEACHPOINT)
							buffer[index] = genBeach(r, c);
						else
							buffer[index] = genConiferousForest(r, c);
					}
					else if (climate[index] < 0.44) { //Chilly to cool
						if (elevation[index] < BEACHPOINT)
							buffer[index] = genBeach(r, c);
						else
							buffer[index] = genMixedForest(r, c);
					}
					else if (climate[index] < 0.55) { //Cool
						if (elevation[index] < BEACHPOINT)
							buffer[index] = genBeach(r, c);
						else
							buffer[index] = genDeciduousForest(r, c);
					}
					else if (climate[index] < 0.66) { //Cool to warm
						if (elevation[index] < BEACHPOINT)
							buffer[index] = genBeach(r, c);
						else
							buffer[index] = genDeciduousForest(r, c);
					}
					else if (climate[index] < 0.77) { //Warm
						if (elevation[index] < BEACHPOINT)
							buffer[index] = genBeach(r, c);
						else
							buffer[index] = genDeciduousForest(r, c);
					}
					else if (climate[index] < 0.88) { //Warm to hot
						if (elevation[index] < BEACHPOINT)
							buffer[index] = genBeach(r, c);
						else
							buffer[index] = genMuddyForest(r, c);
					}
					else { //Hot
						if (elevation[index] < BEACHPOINT)
							buffer[index] = genBeach(r, c);
						else
							buffer[index] = genMonsoon(r, c);
					}

				}
				else if (result < 1.14) { //*************************************************************Watery to wet

					if (climate[index] < 0.2) { //Cold
						if (elevation[index] < BEACHPOINT)
							buffer[index] = genMuddyBeach(r, c);
						else
							buffer[index] = genMuddyConiferousForest(r, c);
					}
					else if (climate[index] < 0.4) { //Chilly
						if (elevation[index] < BEACHPOINT)
							buffer[index] = genBeach(r, c);
						else
							buffer[index] = genMuddyConiferousForest(r, c);
					}
					else if (climate[index] < 0.6) { //Cool
						if (elevation[index] < BEACHPOINT)
							buffer[index] = genBeach(r, c);
						else
							buffer[index] = genGrassyDeciduousForestPlains(r, c);
					}
					else if (climate[index] < 0.8) { //Warm
						if (elevation[index] < BEACHPOINT)
							buffer[index] = genMuddyBeach(r, c);
						else
							buffer[index] = genMuddyForest(r, c);
					}
					else { //Hot
						if (elevation[index] < BEACHPOINT)
							buffer[index] = genMuddyBeach(r, c);
						else
							buffer[index] = genSeasonalRainforest(r, c);
					}

				}
				else { //*******************************************************************************Wet

					if (climate[index] < 0.11) { //Cold
						if (elevation[index] < BEACHPOINT)
							buffer[index] = genConiferousSaltSwamp(r, c);
						else
							buffer[index] = genConiferousSwamp(r, c);
					}
					else if (climate[index] < 0.22) { //Cold to chilly
						if (elevation[index] < BEACHPOINT)
							buffer[index] = genMuddyBeach(r, c);
						else
							buffer[index] = genConiferousSwamp(r, c);
					}
					else if (climate[index] < 0.33) { //Chilly
						if (elevation[index] < BEACHPOINT)
							buffer[index] = genMuddyBeach(r, c);
						else
							buffer[index] = genMuddyConiferousForest(r, c);
					}
					else if (climate[index] < 0.44) { //Chilly to cool
						if (elevation[index] < BEACHPOINT)
							buffer[index] = genMuddyBeach(r, c);
						else
							buffer[index] = genMuddyGrasslandsConiferousForest(r, c);
					}
					else if (climate[index] < 0.55) { //Cool
						if (elevation[index] < BEACHPOINT)
							buffer[index] = genBeach(r, c);
						else
							buffer[index] = genTallGrasslands(r, c);
					}
					else if (climate[index] < 0.66) { //Cool to warm
						if (elevation[index] < BEACHPOINT)
							buffer[index] = genMuddyBeach(r, c);
						else
							buffer[index] = genMarshyGrasslands(r, c);
					}
					else if (climate[index] < 0.77) { //Warm
						if (elevation[index] < BEACHPOINT)
							buffer[index] = genSaltSwamp(r, c);
						else
							buffer[index] = genSwamp(r, c);
					}
					else if (climate[index] < 0.88) { //Warm to hot
						if (elevation[index] < BEACHPOINT)
							buffer[index] = genSaltSwamp(r, c);
						else
							buffer[index] = genMuddySwamp(r, c);
					}
					else { //Hot
						if (elevation[index] < BEACHPOINT)
							buffer[index] = genSaltSwamp(r, c);
						else
							buffer[index] = genEvergreenRainforest(r, c);
					}

				}

			}

			moisture[index] = result;

		}

	}


	char sizeX[] = { lengthX % 256,
					(lengthX / 256) % 256,
					(lengthX / 65536) % 256,
					(lengthX / 16777216) }; //Lengths are stored in reverse
	char sizeY[] = { lengthY % 256,
					(lengthY / 256) % 256,
					(lengthY / 65536) % 256,
					(lengthY / 16777216) };

	earthFile.write(sizeX, 4); earthFile.write(sizeY, 4);
	earthFile.write(buffer, length);
	earthFile.close();
	delete[] buffer;
	delete[] elevation;
	delete[] moisture;
	delete[] climate;
}

double normalFunction(double x, double mean, double deviation) { //Returns a value on the normal distribution

	double sqrt2PI = 2.50662827463; //Saves some time during calculation
	double invsqrtE = 0.60653065971;
	return (1.0 / (deviation * sqrt2PI)) * pow(invsqrtE, pow((x - mean) / deviation, 2));
}

double normalizeToRange(double ix1, double ix2, double tx1, double tx2, double i) {
	double m = (tx2 - tx1) / (ix2 - ix1);
	double dist = tx2 - (ix2 * m);
	return (i * m) + dist;
}



int genDesert(int r, int c) {
	return 4; //Desert 1
}

int genAridGrasslands(int r, int c) {
	int roll = rand() % 2;
	if (roll == 0)
		return 5; //Arid Plains 1
	else
		return 6; //Grasslands 1
}

int genGrasslands(int r, int c) {
	return 6;
}

int genSavanna(int r, int c) {
	return 7; //Savanna 1
}

int genPlains(int r, int c) {
	return 8; //Plains 1
}

int genMonsoonPlains(int r, int c) {
	int roll = rand() % 2;
	if (roll == 0)
		return 9; //Monsoon 1
	else
		return 8;
}

int genMonsoon(int r, int c) {
	return 9;
}

int genSeasonalRainforest(int r, int c) {
	return 10; //Seasonal Rainforest 1
}

int genEvergreenRainforest(int r, int c) {
	return 11; //Evergreen Rainforest 1
}

int genMuddyForest(int r, int c) {
	int roll = rand() % 2;
	if (roll == 0)
		return 12; //Mud 1
	else
		return 13; //Deciduous Forest 1
}

int genMuddySwamp(int r, int c) {
	int roll = rand() % 2;
	if (roll == 0)
		return 14; //Swamp 1
	else
		return 12;
}

int genGrassyPlains(int r, int c) {
	int roll = rand() % 2;
	if (roll == 0)
		return 6;
	else
		return 8;
}

int genDeciduousForestPlains(int r, int c) {
	int roll = rand() % 2;
	if (roll == 0)
		return 13;
	else
		return 8;
}

int genDeciduousForest(int r, int c) {
	return 13;
}

int genSwamp(int r, int c) {
	return 14;
}

int genAridPlains(int r, int c) {
	return 5;
}

int genMarshyGrasslands(int r, int c) {
	int roll = rand() % 2;
	if (roll == 0)
		return 12;
	else
		return 6;
}

int genGrassyDeciduousForestPlains(int r, int c) {
	int roll = rand() % 3;
	if (roll == 0)
		return 6; //Grasslands 1
	else if (roll == 1)
		return 13; //Deciduous Forest 1
	else
		return 8; //Plains 1
}

int genTallGrasslands(int r, int c) {
	return 15; //Tall Grasslands 1
}

int genRockyPlains(int r, int c) {
	int roll = rand() % 2;
	if (roll == 0)
		return 16; //Rocks 1
	else
		return 8;
}

int genConiferousForestPlains(int r, int c) {
	int roll = rand() % 2;
	if (roll == 0)
		return 17; //Coniferous Forest 1
	else
		return 8;
}

int genMixedForestPlains(int r, int c) {
	int roll = rand() % 3;
	if (roll == 0)
		return 17;
	else if (roll == 1)
		return 13;
	else
		return 8;
}

int genMixedForest(int r, int c) {
	int roll = rand() % 2;
	if (roll == 0)
		return 17;
	else
		return 13;
}

int genMuddyGrasslandsConiferousForest(int r, int c) {
	int roll = rand() % 3;
	if (roll == 0)
		return 17; //Coniferous Forest 1
	else if (roll == 1)
		return 6; //Grasslands 1
	else
		return 12; //Mud 1
}
int genRockyConiferousForestPlains(int r, int c) {
	int roll = rand() % 3;
	if (roll == 0)
		return 17;
	else if (roll == 1)
		return 16;
	else
		return 8;
}

int genTaiga(int r, int c) {
	return 18; //Taiga 1
}

int genConiferousForest(int r, int c) {
	return 17;
}

int genMuddyConiferousForest(int r, int c) {
	int roll = rand() % 2;
	if (roll == 0)
		return 12;
	else
		return 17;
}

int genRockySnowyPlains(int r, int c) {
	int roll = rand() % 3;
	if (roll == 0)
		return 16;
	else if (roll == 1)
		return 19; //Snowy 1
	else
		return 8;
}

int genSnowyTaiga(int r, int c) {
	int roll = rand() % 2;
	if (roll == 0)
		return 18;
	else
		return 19;
}

int genConiferousSwamp(int r, int c) {
	int roll = rand() % 2;
	if (roll == 0)
		return 14; //Swamp 1
	else
		return 17; //Coniferous Forest 1
}

int genIceSheet(int r, int c) {
	return 20; //Ice 1
}

int genRockyTundra(int r, int c) {
	int roll = rand() % 2;
	if (roll == 0)
		return 16; //Rocks 1
	else
		return 19; //Snowy 1
}

int genTundra(int r, int c) {
	return 19;
}



int genBeach(int r, int c) {
	return 21; //Beach 1
}

int genMuddyBeach(int r, int c) {
	return 22; //Muddy Beach 1
}

int genSaltSwamp(int r, int c) {
	return 23; //Salt Swamp 1
}

int genRockyBeach(int r, int c) {
	int roll = rand() % 2;
	if (roll == 0)
		return 16; //Rocks 1
	else
		return 21; //Beach 1
}

int genRockyCliff(int r, int c) {
	return 16;
}

int genConiferousSaltSwamp(int r, int c) {
	int roll = rand() % 2;
	if (roll == 0)
		return 23; //Salt Swamp 1
	else
		return 17; //Coniferous Forest 1
}