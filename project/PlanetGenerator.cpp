#include <iostream>
#include <ios>
#include <cmath>
#include <fstream>
#include <chrono>
#include <mutex>
#include "PerlinNoise.hpp"
#include "PlanetGenerator.h"
#include "Globals.h"


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


void generateEarth(unsigned int lengthX, unsigned int lengthY, std::string fileLocation, char* buffer, float* elevation,
				   float* moisture, float* climate, bool& inProgress, int &progress, int &stage, unsigned int seed, 
				   std::mutex* phone, bool useMutex)
{

	if (seed == 0) {
		auto t = std::chrono::system_clock::now();
		srand(t.time_since_epoch().count());
	}
	else
		srand(seed);

	siv::PerlinNoise::seed_type continentSeed = rand();
	siv::PerlinNoise::seed_type islandSeed = rand();
	siv::PerlinNoise::seed_type moistureSeed = rand();

	siv::PerlinNoise cgen{ continentSeed };
	siv::PerlinNoise igen{ islandSeed };
	siv::PerlinNoise mgen{ moistureSeed };

	unsigned int length = lengthX * lengthY;
	bool polarX = loop_x;
	bool polarY = loop_y;

	//Continent variables
	float LANDCRUNCHER = land_cruncher; //Larger means beefier continents
	float ISLANDCRUNCHER = island_cruncher; //Larger means beefier islands
	float LANDPOINT = land_point; //Same effect as increasing landcruncher
	float SPARSITY = sparsity; //Larger means more space between land and sea
	float ISPARSITY = i_sparsity; //Sparsity, but for islands
	float IPOINT = i_point; //The point at which an island is most likely to spawn; farther from landpoint means farther from land
	float IPUNISH = i_punish; //Larger means the island will become more submerged the farther it gets from IPOINT;

	//Climate variables
	bool USINGEQUATOR = using_equator;
	float CLIMATECRUNCHER = climate_cruncher; //Larger means the climate will be more "extreme"
	float RANGESTART = range_start; //The wider the range of these two variables, the more "smooth" equator heat generation will be
	float RANGEEND = range_stop;
	float MAXMODIFY = max_modify; //The largest value that will be added to the range; the larger it is the more "hotter" the equator will be

	//Biome variables
	float MOISTURECRUNCHER = moisture_cruncher;
	float BEACHPOINT = beach_point; //Larger means more and more land will be a beach biome

	const float TWOPI = 6.2832;
	float angleRadiusX = (1.0 / SPARSITY) / (TWOPI / float(lengthX));
	float angleRadiusY = (1.0 / SPARSITY) / (TWOPI / float(lengthY));
	float iAngleRadiusX = (1.0 / ISPARSITY) / (TWOPI / float(lengthX));
	float iAngleRadiusY = (1.0 / ISPARSITY) / (TWOPI / float(lengthY));

	float equator = (lengthY - 1.0) / 2.0; //The vertical line of the map
	float modifier = 3.00; //Makes sure the edges of the map are always water for looping
	float limit = 0.50;
	float borderX = (lengthX - 1.0) * limit;
	float borderY = (lengthY - 1.0) * limit;



	std::cout << "Generating terrain" << std::endl;
	for (int r = 0; r < lengthY; r++) { //Continent + island generation

		float angleY = 0.f;
		if (polarY)
			angleY = (r / float(lengthY)) * TWOPI;

		for (int c = 0; c < lengthX; c++) {

			float angleX = 0.f;
			if (polarX)
				angleX = (c / float(lengthX)) * TWOPI;

			float result = 0.f;
			if (polarX && polarY) {
				float pX1 = cos(angleX); float pY1 = sin(angleX);
				float pX2 = cos(angleY); float pY2 = sin(angleY);
				result = cgen.noise4D(pX1 * angleRadiusX, pY1 * angleRadiusX, pX2 * angleRadiusY, pY2 * angleRadiusY) + cgen.octave4D(pX1 * angleRadiusX, pY1 * angleRadiusX, pX2 * angleRadiusY, pY2 * angleRadiusY, 10);
				result *= LANDCRUNCHER;
			}
			else if (polarX) {
				float pX = cos(angleX); float pY = sin(angleX);
				result = cgen.noise3D(pX * angleRadiusX, pY * angleRadiusX, r / SPARSITY) + cgen.octave3D(pX * angleRadiusX, pY * angleRadiusX, r / SPARSITY, 10);
				result *= LANDCRUNCHER; //Scaled
			}
			else if (polarY) {
				float pX = cos(angleY); float pY = sin(angleY);
				result = cgen.noise3D(pX * angleRadiusY, pY * angleRadiusY, c / SPARSITY) + cgen.octave3D(pX * angleRadiusY, pY * angleRadiusY, c / SPARSITY, 10);
				result *= LANDCRUNCHER;
			}
			else { //No looping
				result = cgen.noise2D(c / SPARSITY, r / SPARSITY) + cgen.octave2D(c / SPARSITY, r / SPARSITY, 10);
				result *= LANDCRUNCHER;
			}


			int index = c + (r * lengthX);
			int output = 0;

			if (result < LANDPOINT - 0.08f) {

				float shoreFactor = IPUNISH * std::abs((IPOINT - result));
				if (polarX && polarY) {
					float pX1 = cos(angleX); float pY1 = sin(angleX);
					float pX2 = cos(angleY); float pY2 = sin(angleY);
					result = igen.noise4D(pX1 * iAngleRadiusX, pY1 * iAngleRadiusX, pX2 * iAngleRadiusY, pY2 * iAngleRadiusY) + igen.octave4D(pX1 * iAngleRadiusX, pY1 * iAngleRadiusX, pX2 * iAngleRadiusY, pY2 * iAngleRadiusY, 4);
					result *= ISLANDCRUNCHER;
				}
				else if (polarX) {
					float pX = cos(angleX); float pY = sin(angleX);
					result = igen.noise3D(pX * iAngleRadiusX, pY * iAngleRadiusX, r / ISPARSITY) + igen.octave3D(pX * iAngleRadiusX, pY * iAngleRadiusX, r / ISPARSITY, 4);
					result *= ISLANDCRUNCHER;
				}
				else if (polarY) {
					float pX = cos(angleY); float pY = sin(angleY);
					result = igen.noise3D(pX * iAngleRadiusY, pY * iAngleRadiusY, c / ISPARSITY) + igen.octave3D(pX * iAngleRadiusY, pY * iAngleRadiusY, c / ISPARSITY, 4);
					result *= ISLANDCRUNCHER;
				}
				else {
					result = igen.noise2D(c / ISPARSITY, r / ISPARSITY) + igen.octave2D(c / ISPARSITY, r / ISPARSITY, 4);
					result *= ISLANDCRUNCHER;
				}
				result -= shoreFactor;
				result = result / 4.f; //Scaling

				if (result < LANDPOINT - 0.08f)
					output = 0;
				else if (result < LANDPOINT - 0.03f)
					output = 1;
				else if (result < LANDPOINT)
					output = 2;
				else
					output = 3;

			}
			else if (result < LANDPOINT - 0.03f) {
				output = 1; //Shallowish water
			}
			else if (result < LANDPOINT) {
				output = 2; //Shallow water
			}
			else
				output = 3; //Land

			elevation[index] = result;


			if (useMutex) phone->lock(); //Danger zone
			if (!inProgress)
			{
				if (useMutex) phone->unlock(); //Abort
				return;
			}
			buffer[index] = (char)output;
			progress++;
			if (useMutex) phone->unlock();
		}
	}

	

	std::cout << "Generating heat" << std::endl;
	siv::PerlinNoise::seed_type climateSeed = rand();
	siv::PerlinNoise clgen{ climateSeed };
	//double factor = 1.0 / normalFunction(equator, equator, 100.0); //Range is from 1 to 0

	for (int r = 0; r < lengthY; r++) { //Climate mapping

		float angleY = 0.f;
		if (polarY)
			angleY = (r / float(lengthY)) * TWOPI;

		//double heatValue = normalFunction(r, equator, 100.0) * factor;
		//double heatValue = 1.0 - (abs(r - equator) / equator);

		float modify = 0.f;
		if (USINGEQUATOR) {
			if (r < equator)
				modify = MAXMODIFY * (r / equator);
			else
				modify = MAXMODIFY * ((equator - (r - equator)) / equator);
		}

		for (int c = 0; c < lengthX; c++) {

			float angleX = 0.f;
			if (polarX)
				angleX = (c / float(lengthX)) * TWOPI;

			float heatValue = 0.f;
			if (polarX && polarY) {
				float pX1 = cos(angleX); float pY1 = sin(angleX);
				float pX2 = cos(angleY); float pY2 = sin(angleY);
				heatValue = clgen.noise4D(pX1 * angleRadiusX, pY1 * angleRadiusX, pX2 * angleRadiusY, pY2 * angleRadiusY) + clgen.octave4D(pX1 * angleRadiusX, pY1 * angleRadiusX, pX2 * angleRadiusY, pY2 * angleRadiusY, 6);
				heatValue *= CLIMATECRUNCHER;
			}
			else if (polarX) {
				float pX = cos(angleX); float pY = sin(angleX);
				heatValue = clgen.noise3D(pX * angleRadiusX, pY * angleRadiusX, r / SPARSITY) + clgen.octave3D(pX * angleRadiusX, pY * angleRadiusX, r / SPARSITY, 6);
				heatValue *= CLIMATECRUNCHER;
			}
			else if (polarY) {
				float pX = cos(angleY); float pY = sin(angleY);
				heatValue = clgen.noise3D(pX * angleRadiusY, pY * angleRadiusY, c / SPARSITY) + clgen.octave3D(pX * angleRadiusY, pY * angleRadiusY, c / SPARSITY, 6);
				heatValue *= CLIMATECRUNCHER;
			}
			else {
				heatValue = clgen.noise2D(c / SPARSITY, r / SPARSITY) + clgen.octave2D(c / SPARSITY, r / SPARSITY, 6);
				heatValue *= CLIMATECRUNCHER;
			}
			
			heatValue = normalizeToRange(-1, 1, RANGESTART + modify, RANGEEND + modify, heatValue);

			//double heatModifier = clgen.noise2D(c / 550.0, r / 550.0) + clgen.octave2D(c / 550.0, r / 550.0, 3); //From about -1.52 to 1.52
			//heatValue += (heatModifier * 0.131579 * 0.2);
			int index = c + (r * lengthX);
			climate[index] = heatValue;


			if (useMutex) phone->lock();
			if (!inProgress)
			{
				if (useMutex) phone->unlock();
				return;
			}
			if (useMutex) phone->unlock();
		}
	}

	if (useMutex) phone->lock();
	if (!inProgress)
	{
		if (useMutex) phone->unlock();
		return;
	}
	progress = 0;
	stage++;
	if (useMutex) phone->unlock();



	std::cout << "Generating biomes" << std::endl;
	for (int r = 0; r < lengthY; r++) { //Biome generation

		float angleY = 0.f;
		if (polarY)
			angleY = (r / float(lengthY)) * TWOPI;

		for (int c = 0; c < lengthX; c++) {

			float angleX = 0.f;
			if (polarX)
				angleX = (c / float(lengthX)) * TWOPI;

			int index = c + (r * lengthX);
			int output = buffer[index];

			float result = 0.f;
			if (polarX && polarY) { //TODO; needs 4D Perlin noise
				float pX1 = cos(angleX); float pY1 = sin(angleX);
				float pX2 = cos(angleY); float pY2 = sin(angleY);
				result = mgen.noise4D(pX1 * angleRadiusX, pY1 * angleRadiusX, pX2 * angleRadiusY, pY2 * angleRadiusY) + mgen.octave4D(pX1 * angleRadiusX, pY1 * angleRadiusX, pX2 * angleRadiusY, pY2 * angleRadiusY, 10);
				result *= MOISTURECRUNCHER;
			}
			else if (polarX) {
				float pX = cos(angleX); float pY = sin(angleX);
				result = mgen.noise3D(pX * angleRadiusX, pY * angleRadiusX, r / SPARSITY) + mgen.octave3D(pX * angleRadiusX, pY * angleRadiusX, r / SPARSITY, 10);
				result *= MOISTURECRUNCHER; //Scaled
			}
			else if (polarY) {
				float pX = cos(angleY); float pY = sin(angleY);
				result = mgen.noise3D(pX * angleRadiusY, pY * angleRadiusY, c / SPARSITY) + mgen.octave3D(pX * angleRadiusY, pY * angleRadiusY, c / SPARSITY, 10);
				result *= MOISTURECRUNCHER;
			}
			else { //No looping
				result = mgen.noise2D(c / SPARSITY, r / SPARSITY) + mgen.octave2D(c / SPARSITY, r / SPARSITY, 10);
				result *= MOISTURECRUNCHER;
			}
			result *= 0.955414f;

			if (buffer[index] == 3) { //Land

				if (result < -1.17f) { //******************************************************************Arid

					if (climate[index] < 0.11f) { //Cold
						output = genIceSheet(r, c);
					}
					else if (climate[index] < 0.22f) { //Cold to chilly
						if (elevation[index] < BEACHPOINT)
							output = genRockyCliff(r, c);
						else
							output = genRockySnowyPlains(r, c);
					}
					else if (climate[index] < 0.33f) { //Chilly
						if (elevation[index] < BEACHPOINT)
							output = genRockyCliff(r, c);
						else
							output = genRockyPlains(r, c);
					}
					else if (climate[index] < 0.44f) { //Chilly to cool
						if (elevation[index] < BEACHPOINT)
							output = genRockyBeach(r, c);
						else
							output = genRockyPlains(r, c);
					}
					else if (climate[index] < 0.55f) { //Cool
						if (elevation[index] < BEACHPOINT)
							output = genBeach(r, c);
						else
							output = genPlains(r, c);
					}
					else if (climate[index] < 0.66f) { //Cool to warm
						if (elevation[index] < BEACHPOINT)
							output = genBeach(r, c);
						else
							output = genAridPlains(r, c);
					}
					else if (climate[index] < 0.77f) { //Warm
						output = genDesert(r, c);
					}
					else if (climate[index] < 0.88f) { //Warm to hot
						output = genDesert(r, c);
					}
					else { //Hot
						output = genDesert(r, c);
					}

				}
				else if (result < -0.84f) { //*************************************************************Arid to dry

					if (climate[index] < 0.2f) { //Cold
						if (elevation[index] < BEACHPOINT)
							output = genRockyCliff(r, c);
						else
							output = genRockyTundra(r, c);
					}
					else if (climate[index] < 0.4f) { //Chilly
						if (elevation[index] < BEACHPOINT)
							output = genRockyBeach(r, c);
						else
							output = genRockyConiferousForestPlains(r, c);
					}
					else if (climate[index] < 0.6f) { //Cool
						if (elevation[index] < BEACHPOINT)
							output = genBeach(r, c);
						else
							output = genPlains(r, c);
					}
					else if (climate[index] < 0.8f) { //Warm
						if (elevation[index] < BEACHPOINT)
							output = genBeach(r, c);
						else
							output = genAridGrasslands(r, c);
					}
					else { //Hot
						if (elevation[index] < BEACHPOINT)
							output = genBeach(r, c);
						else
							output = genAridGrasslands(r, c);
					}

				}
				else if (result < -0.51f) { //*************************************************************Dry

					if (climate[index] < 0.11f) { //Cold
						if (elevation[index] < BEACHPOINT)
							output = genRockyCliff(r, c);
						else
							output = genTundra(r, c);
					}
					else if (climate[index] < 0.22f) { //Cold to chilly
						if (elevation[index] < BEACHPOINT)
							output = genRockyBeach(r, c);
						else
							output = genSnowyTaiga(r, c);
					}
					else if (climate[index] < 0.33f) { //Chilly
						if (elevation[index] < BEACHPOINT)
							output = genBeach(r, c);
						else
							output = genTaiga(r, c);
					}
					else if (climate[index] < 0.44f) { //Chilly to cool
						if (elevation[index] < BEACHPOINT)
							output = genBeach(r, c);
						else
							output = genConiferousForestPlains(r, c);
					}
					else if (climate[index] < 0.55f) { //Cool
						if (elevation[index] < BEACHPOINT)
							output = genBeach(r, c);
						else
							output = genPlains(r, c);
					}
					else if (climate[index] < 0.66f) { //Cool to warm
						if (elevation[index] < BEACHPOINT)
							output = genBeach(r, c);
						else
							output = genGrassyPlains(r, c);
					}
					else if (climate[index] < 0.77f) { //Warm
						if (elevation[index] < BEACHPOINT)
							output = genBeach(r, c);
						else
							output = genGrasslands(r, c);
					}
					else if (climate[index] < 0.88f) { //Warm to hot
						if (elevation[index] < BEACHPOINT)
							output = genBeach(r, c);
						else
							output = genGrasslands(r, c);
					}
					else { //Hot
						if (elevation[index] < BEACHPOINT)
							output = genBeach(r, c);
						else
							output = genGrasslands(r, c);
					}

				}
				else if (result < -0.18f) { //*************************************************************Dry to normal

					if (climate[index] < 0.2f) { //Cold
						if (elevation[index] < BEACHPOINT)
							output = genRockyBeach(r, c);
						else
							output = genSnowyTaiga(r, c);
					}
					else if (climate[index] < 0.4f) { //Chilly
						if (elevation[index] < BEACHPOINT)
							output = genBeach(r, c);
						else
							output = genConiferousForest(r, c);
					}
					else if (climate[index] < 0.6f) { //Cool
						if (elevation[index] < BEACHPOINT)
							output = genBeach(r, c);
						else
							output = genDeciduousForestPlains(r, c);
					}
					else if (climate[index] < 0.8f) { //Warm
						if (elevation[index] < BEACHPOINT)
							output = genBeach(r, c);
						else
							output = genGrassyPlains(r, c);
					}
					else { //Hot
						if (elevation[index] < BEACHPOINT)
							output = genBeach(r, c);
						else
							output = genSavanna(r, c);
					}

				}
				else if (result < 0.15f) { //*************************************************************Normal

					if (climate[index] < 0.11f) { //Cold
						if (elevation[index] < BEACHPOINT)
							output = genBeach(r, c);
						else
							output = genTaiga(r, c);
					}
					else if (climate[index] < 0.22f) { //Cold to chilly
						if (elevation[index] < BEACHPOINT)
							output = genBeach(r, c);
						else
							output = genConiferousForest(r, c);
					}
					else if (climate[index] < 0.33f) { //Chilly
						if (elevation[index] < BEACHPOINT)
							output = genBeach(r, c);
						else
							output = genConiferousForest(r, c);
					}
					else if (climate[index] < 0.44f) { //Chilly to cool
						if (elevation[index] < BEACHPOINT)
							output = genBeach(r, c);
						else
							output = genMixedForestPlains(r, c);
					}
					else if (climate[index] < 0.55f) { //Cool
						if (elevation[index] < BEACHPOINT)
							output = genBeach(r, c);
						else
							output = genDeciduousForestPlains(r, c);
					}
					else if (climate[index] < 0.66f) { //Cool to warm
						if (elevation[index] < BEACHPOINT)
							output = genBeach(r, c);
						else
							output = genDeciduousForestPlains(r, c);
					}
					else if (climate[index] < 0.77f) { //Warm
						if (elevation[index] < BEACHPOINT)
							output = genBeach(r, c);
						else
							output = genPlains(r, c);
					}
					else if (climate[index] < 0.88f) { //Warm to hot
						if (elevation[index] < BEACHPOINT)
							output = genBeach(r, c);
						else
							output = genPlains(r, c);
					}
					else { //Hot
						if (elevation[index] < BEACHPOINT)
							output = genBeach(r, c);
						else
							output = genPlains(r, c);
					}

				}
				else if (result < 0.48f) { //*************************************************************Normal to watery

					if (climate[index] < 0.2f) { //Cold
						if (elevation[index] < BEACHPOINT)
							output = genBeach(r, c);
						else
							output = genConiferousForest(r, c);
					}
					else if (climate[index] < 0.4f) { //Chilly
						if (elevation[index] < BEACHPOINT)
							output = genBeach(r, c);
						else
							output = genConiferousForest(r, c);
					}
					else if (climate[index] < 0.6f) { //Cool
						if (elevation[index] < BEACHPOINT)
							output = genBeach(r, c);
						else
							output = genDeciduousForest(r, c);
					}
					else if (climate[index] < 0.8f) { //Warm
						if (elevation[index] < BEACHPOINT)
							output = genBeach(r, c);
						else
							output = genDeciduousForestPlains(r, c);
					}
					else { //Hot
						if (elevation[index] < BEACHPOINT)
							output = genBeach(r, c);
						else
							output = genMonsoonPlains(r, c);
					}

				}
				else if (result < 0.81f) { //*************************************************************Watery

					if (climate[index] < 0.11f) { //Cold
						if (elevation[index] < BEACHPOINT)
							output = genBeach(r, c);
						else
							output = genConiferousForest(r, c);
					}
					else if (climate[index] < 0.22f) { //Cold to chilly
						if (elevation[index] < BEACHPOINT)
							output = genBeach(r, c);
						else
							output = genConiferousForest(r, c);
					}
					else if (climate[index] < 0.33f) { //Chilly
						if (elevation[index] < BEACHPOINT)
							output = genBeach(r, c);
						else
							output = genConiferousForest(r, c);
					}
					else if (climate[index] < 0.44f) { //Chilly to cool
						if (elevation[index] < BEACHPOINT)
							output = genBeach(r, c);
						else
							output = genMixedForest(r, c);
					}
					else if (climate[index] < 0.55f) { //Cool
						if (elevation[index] < BEACHPOINT)
							output = genBeach(r, c);
						else
							output = genDeciduousForest(r, c);
					}
					else if (climate[index] < 0.66f) { //Cool to warm
						if (elevation[index] < BEACHPOINT)
							output = genBeach(r, c);
						else
							output = genDeciduousForest(r, c);
					}
					else if (climate[index] < 0.77f) { //Warm
						if (elevation[index] < BEACHPOINT)
							output = genBeach(r, c);
						else
							output = genDeciduousForest(r, c);
					}
					else if (climate[index] < 0.88f) { //Warm to hot
						if (elevation[index] < BEACHPOINT)
							output = genBeach(r, c);
						else
							output = genMuddyForest(r, c);
					}
					else { //Hot
						if (elevation[index] < BEACHPOINT)
							output = genBeach(r, c);
						else
							output = genMonsoon(r, c);
					}

				}
				else if (result < 1.14f) { //*************************************************************Watery to wet

					if (climate[index] < 0.2f) { //Cold
						if (elevation[index] < BEACHPOINT)
							output = genMuddyBeach(r, c);
						else
							output = genMuddyConiferousForest(r, c);
					}
					else if (climate[index] < 0.4f) { //Chilly
						if (elevation[index] < BEACHPOINT)
							output = genBeach(r, c);
						else
							output = genMuddyConiferousForest(r, c);
					}
					else if (climate[index] < 0.6f) { //Cool
						if (elevation[index] < BEACHPOINT)
							output = genBeach(r, c);
						else
							output = genGrassyDeciduousForestPlains(r, c);
					}
					else if (climate[index] < 0.8f) { //Warm
						if (elevation[index] < BEACHPOINT)
							output = genMuddyBeach(r, c);
						else
							output = genMuddyForest(r, c);
					}
					else { //Hot
						if (elevation[index] < BEACHPOINT)
							output = genMuddyBeach(r, c);
						else
							output = genSeasonalRainforest(r, c);
					}

				}
				else { //*******************************************************************************Wet

					if (climate[index] < 0.11f) { //Cold
						if (elevation[index] < BEACHPOINT)
							output = genConiferousSaltSwamp(r, c);
						else
							output = genConiferousSwamp(r, c);
					}
					else if (climate[index] < 0.22f) { //Cold to chilly
						if (elevation[index] < BEACHPOINT)
							output = genMuddyBeach(r, c);
						else
							output = genConiferousSwamp(r, c);
					}
					else if (climate[index] < 0.33f) { //Chilly
						if (elevation[index] < BEACHPOINT)
							output = genMuddyBeach(r, c);
						else
							output = genMuddyConiferousForest(r, c);
					}
					else if (climate[index] < 0.44f) { //Chilly to cool
						if (elevation[index] < BEACHPOINT)
							output = genMuddyBeach(r, c);
						else
							output = genMuddyGrasslandsConiferousForest(r, c);
					}
					else if (climate[index] < 0.55f) { //Cool
						if (elevation[index] < BEACHPOINT)
							output = genBeach(r, c);
						else
							output = genTallGrasslands(r, c);
					}
					else if (climate[index] < 0.66f) { //Cool to warm
						if (elevation[index] < BEACHPOINT)
							output = genMuddyBeach(r, c);
						else
							output = genMarshyGrasslands(r, c);
					}
					else if (climate[index] < 0.77f) { //Warm
						if (elevation[index] < BEACHPOINT)
							output = genSaltSwamp(r, c);
						else
							output = genSwamp(r, c);
					}
					else if (climate[index] < 0.88f) { //Warm to hot
						if (elevation[index] < BEACHPOINT)
							output = genSaltSwamp(r, c);
						else
							output = genMuddySwamp(r, c);
					}
					else { //Hot
						if (elevation[index] < BEACHPOINT)
							output = genSaltSwamp(r, c);
						else
							output = genEvergreenRainforest(r, c);
					}

				}

			}

			moisture[index] = result;


			if (useMutex) phone->lock();
			if (!inProgress)
			{
				if (useMutex) phone->unlock();
				return;
			}
			buffer[index] = (char)output;
			progress++;
			if (useMutex) phone->unlock();
		}

	}



	std::cout << "Complete" << std::endl;

	if (useMutex) phone->lock();
	inProgress = false;
	if (useMutex) phone->unlock();
}

double normalFunction(double x, double mean, double deviation) { //Returns a value on the normal distribution

	double sqrt2PI = 2.50662827463; //Saves some time during calculation
	double invsqrtE = 0.60653065971;
	return (1.0 / (deviation * sqrt2PI)) * pow(invsqrtE, pow((x - mean) / deviation, 2));
}

float normalizeToRange(float ix1, float ix2, float tx1, float tx2, float i) {
	float m = (tx2 - tx1) / (ix2 - ix1);
	float dist = tx2 - (ix2 * m);
	return (i * m) + dist;
}



void returnRGBA(int val, int* rgb)
{
	switch (val) //Different coloring
	{
		case 0:
			rgb[0] = 0;
			rgb[1] = 0;
			rgb[2] = 130;
			rgb[3] = 255;
			break;
		case 1:
			rgb[0] = 0;
			rgb[1] = 0;
			rgb[2] = 207;
			rgb[3] = 255;
			break;
		case 2:
			rgb[0] = 0;
			rgb[1] = 0;
			rgb[2] = 255;
			rgb[3] = 255;
			break;
		case 3:
			rgb[0] = 0;
			rgb[1] = 200;
			rgb[2] = 0;
			rgb[3] = 255;
			break;
		case 4:
			rgb[0] = 247;
			rgb[1] = 247;
			rgb[2] = 116;
			rgb[3] = 255;
			break;
		case 5:
			rgb[0] = 184;
			rgb[1] = 184;
			rgb[2] = 70;
			rgb[3] = 255;
			break;
		case 6:
			rgb[0] = 232;
			rgb[1] = 115;
			rgb[2] = 60;
			rgb[3] = 255;
			break;
		case 7:
			rgb[0] = 245;
			rgb[1] = 107;
			rgb[2] = 27;
			rgb[3] = 255;
			break;
		case 8:
			rgb[0] = 69;
			rgb[1] = 196;
			rgb[2] = 43;
			rgb[3] = 255;
			break;
		case 9:
			rgb[0] = 43;
			rgb[1] = 0;
			rgb[2] = 0;
			rgb[3] = 255;
			break;
		case 10:
			rgb[0] = 0;
			rgb[1] = 133;
			rgb[2] = 13;
			rgb[3] = 255;
			break;
		case 11:
			rgb[0] = 0;
			rgb[1] = 138;
			rgb[2] = 13;
			rgb[3] = 255;
			break;
		case 12:
			rgb[0] = 117;
			rgb[1] = 0;
			rgb[2] = 0;
			rgb[3] = 255;
			break;
		case 13:
			rgb[0] = 9;
			rgb[1] = 61;
			rgb[2] = 0;
			rgb[3] = 255;
			break;
		case 14:
			rgb[0] = 11;
			rgb[1] = 33;
			rgb[2] = 7;
			rgb[3] = 255;
			break;
		case 15:
			rgb[0] = 196;
			rgb[1] = 130;
			rgb[2] = 49;
			rgb[3] = 255;
			break;
		case 16:
			rgb[0] = 125;
			rgb[1] = 123;
			rgb[2] = 120;
			rgb[3] = 255;
			break;
		case 17:
			rgb[0] = 50;
			rgb[1] = 84;
			rgb[2] = 47;
			rgb[3] = 255;
			break;
		case 18:
			rgb[0] = 85;
			rgb[1] = 125;
			rgb[2] = 81;
			rgb[3] = 255;
			break;
		case 19:
			rgb[0] = 238;
			rgb[1] = 230;
			rgb[2] = 255;
			rgb[3] = 255;
			break;
		case 20:
			rgb[0] = 222;
			rgb[1] = 191;
			rgb[2] = 255;
			rgb[3] = 255;
			break;
		case 21:
			rgb[0] = 255;
			rgb[1] = 242;
			rgb[2] = 207;
			rgb[3] = 255;
			break;
		case 22:
			rgb[0] = 212;
			rgb[1] = 197;
			rgb[2] = 159;
			rgb[3] = 255;
			break;
		case 23:
			rgb[0] = 35;
			rgb[1] = 56;
			rgb[2] = 31;
			rgb[3] = 255;
			break;
		default:
			rgb[0] = 0;
			rgb[1] = 0;
			rgb[2] = 0;
			rgb[3] = 255;
			break;
	}
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