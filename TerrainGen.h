#pragma once
#include <list>

typedef struct {
	int x;
	int y;
} vector2;

namespace bgTG {
	void smooth(int** grid_to_smooth, int resolution);
	int** generateComplexDotGrid(int resolution, int ocean_chance, int smoothing_steps, int archepeligo_val, std::list<vector2> veroni_points);
	int** generateSimpleDotGrid(int resolution, int ocean_chance, int smoothing_steps);
	void grow_continents(int** grid_to_grow, int resolution);
	void grow_continents_reverse(int** grid_to_grow, int resolution);
	int** generate_continents(int grid_res, int continent_size, int river_amt, int mtn_amt);
	void create_river(int grid_res, int** grid);
	void generate_mountain(int grid_res, int** grid, int roughness, int max_length, std::list<vector2> veroni_points);
}