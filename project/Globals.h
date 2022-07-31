#pragma once

/*********************************************************************************************
These variables are modified by the GUI and will be taken in by the planet generator function
*********************************************************************************************/

extern unsigned int length_x;
extern unsigned int length_y;
extern bool loop_x;
extern bool loop_y;
extern unsigned int seed;
	
extern float land_cruncher;
extern float island_cruncher;
extern float land_point;
extern float sparsity;
extern float i_sparsity;
extern float i_point;
extern float i_punish;

extern bool using_equator;
extern float climate_cruncher;
extern float range_start;
extern float range_stop;
extern float max_modify;

extern float moisture_cruncher;
extern float beach_point;