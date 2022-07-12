#pragma once
#include <iostream>

void generateEarth(unsigned int chunksX, unsigned int chunksY, unsigned int textureX, unsigned int textureY, std::string fileLocation);
double normalFunction(double x, double mean = 0.0, double deviation = 1.0);
double normalizeToRange(double ix1, double ix2, double tx1, double tx2, double i);

int genDesert(int r, int c);
int genAridGrasslands(int r, int c);
int genGrasslands(int r, int c);
int genSavanna(int r, int c);
int genPlains(int r, int c);
int genMonsoonPlains(int r, int c);
int genMonsoon(int r, int c);
int genSeasonalRainforest(int r, int c);
int genEvergreenRainforest(int r, int c);
int genMuddyForest(int r, int c);
int genMuddySwamp(int r, int c);
int genGrassyPlains(int r, int c);
int genDeciduousForestPlains(int r, int c);
int genDeciduousForest(int r, int c);
int genSwamp(int r, int c);
int genAridPlains(int r, int c);
int genMarshyGrasslands(int r, int c);
int genGrassyDeciduousForestPlains(int r, int c);
int genTallGrasslands(int r, int c);
int genRockyPlains(int r, int c);
int genConiferousForestPlains(int r, int c);
int genMixedForestPlains(int r, int c);
int genMixedForest(int r, int c);
int genMuddyGrasslandsConiferousForest(int r, int c);
int genRockyConiferousForestPlains(int r, int c);
int genTaiga(int r, int c);
int genConiferousForest(int r, int c);
int genMuddyConiferousForest(int r, int c);
int genRockySnowyPlains(int r, int c);
int genSnowyTaiga(int r, int c);
int genConiferousSwamp(int r, int c);
int genIceSheet(int r, int c);
int genRockyTundra(int r, int c);
int genTundra(int r, int c);

int genBeach(int r, int c);
int genMuddyBeach(int r, int c);
int genSaltSwamp(int r, int c);
int genRockyBeach(int r, int c);
int genRockyCliff(int r, int c);
int genConiferousSaltSwamp(int r, int c);