#pragma once

/*********************************************************************************************
These variables are modified by the GUI and will be taken in by the planet generator function
*********************************************************************************************/

extern unsigned int length_x;
extern unsigned int length_y;
extern bool loop_x;
extern bool loop_y;
extern unsigned int seed;
	
extern double land_cruncher;
extern double island_cruncher;
extern double land_point;
extern double sparsity;
extern double i_sparsity;
extern double i_point;
extern double i_punish;

extern bool using_equator;
extern double climate_cruncher;
extern double range_start;
extern double range_stop;
extern double max_modify;

extern double moisture_cruncher;
extern double beach_point;