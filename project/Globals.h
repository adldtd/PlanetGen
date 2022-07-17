#pragma once

/*********************************************************************************************
These variables are modified by the GUI and will be taken in by the planet generator function
*********************************************************************************************/

namespace guivars
{
	unsigned int length_x;
	unsigned int length_y;
	bool loop_x;
	bool loop_y;
	unsigned int seed;
	
	double land_cruncher;
	double island_cruncher;
	double land_point;
	double sparsity;
	double i_sparsity;
	double i_point;
	double i_punish;

	bool using_equator;
	double climate_cruncher;
	double range_start;
	double range_end;
	double max_modify;

	double moisture_cruncher;
	double beach_point;
}