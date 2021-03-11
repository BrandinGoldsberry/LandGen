#include "TerrainGen.h"
#include <math.h>
#include <stdlib.h>

int distance(vector2 coords, vector2 center) {
    int square_1 = (coords.x - center.x) * (coords.x - center.x);
    int square_2 = (coords.y - center.y) * (coords.y - center.y);

    int form_step_1 = abs(square_1 + square_2);
    float form_step_2 = round(sqrt(form_step_1));
    return (int)form_step_2;
}

vector2 find_midpoint(vector2 a, vector2 b) {
    return vector2{ ((a.x + b.x) / 2), ((a.y + b.y) / 2) };
}

vector2 find_shortest_distance(std::list<vector2> points, vector2 location) {
    int last_shortest_length = INT_MAX;
    vector2 ret;
    for (auto i = points.begin(); i != points.end(); i++)
    {
        int cur_dist = distance(*i, location);
        if (last_shortest_length > cur_dist) {
            last_shortest_length = cur_dist;
            ret = *i;
        }
    }

    return ret;
}

int* find_coords_on_square(int input_dir) {
    int* ret = new int[2];

    double circle_coord_x = sin((input_dir * 45.00));
    double circle_coord_y = cos((input_dir * 45.00));

    if (circle_coord_x < 0) {
        ret[0] = floor(circle_coord_x);
    }
    else {
        ret[0] = ceil(circle_coord_x);
    }
    if (circle_coord_y < 0) {
        ret[1] = floor(circle_coord_y);
    }
    else {
        ret[1] = ceil(circle_coord_y);
    }

    return ret;
}

void bgTG::smooth(int** grid_to_smooth, int resolution)
{
    for (int i = 0; i < resolution; i++)
    {
        for (int j = 0; j < resolution; j++)
        {
            int ocean_count = 0;
            if (i > 0) {
                ocean_count += grid_to_smooth[i - 1][j] == 0;
            }
            if (i < resolution - 1) {
                ocean_count += grid_to_smooth[i + 1][j] == 0;
            }
            if (j > 0) {
                ocean_count += grid_to_smooth[i][j - 1] == 0;
            }
            if (j < resolution - 1) {
                ocean_count += grid_to_smooth[i][j + 1] == 0;
            }
            if (i > 0 && j > 0) {
                ocean_count += grid_to_smooth[i - 1][j - 1] == 0;
            }
            if (i < resolution - 1 && j > 0) {
                ocean_count += grid_to_smooth[i + 1][j - 1] == 0;
            }
            if (i > 0 && j < resolution - 1) {
                ocean_count += grid_to_smooth[i - 1][j + 1] == 0;
            }
            if (i < resolution - 1 && j < resolution - 1) {
                ocean_count += grid_to_smooth[i + 1][j + 1] == 0;
            }
            if (ocean_count < 4)
            {
                if (grid_to_smooth[i][j] != 3) {
                    grid_to_smooth[i][j] = 2;

                }
            }
            else if (ocean_count > 4) {
                if (grid_to_smooth[i][j] != 3) {
                    grid_to_smooth[i][j] = 0;
                }
            }
        }
    }
}

std::list<vector2> generate_worely_noise_grid(int point_num, int width) {
    std::list<vector2> veroni_points;
    for (int i = 0; i < point_num; i++)
    {
        int res_center = ((width * 3) / 4);
        int max_res = (width * 3) / 16;
        int rand_x = (rand() % res_center) + max_res;
        int rand_y = (rand() % res_center) + max_res;
        veroni_points.push_back(vector2{ rand_x, rand_y });
    }

    return veroni_points;
}

int** bgTG::generateComplexDotGrid(int resolution, int land_chance, int smoothing_steps, int archepeligo_val, std::list<vector2> veroni_points)
{
    int veroni_res = 10;
    int grid_buffer = resolution / 50;
    int farthest_distance = (distance(vector2{ 0, resolution / 2 }, vector2{ resolution / 2, resolution / 2 }));

    //the terrain to use
    int** newGrid = 0;

    newGrid = new int*[resolution];
    for (int i = 0; i < resolution; i++)
    {
        newGrid[i] = new int[resolution];
        for (int j = 0; j < resolution; j++)
        {
            if (j < grid_buffer || i < grid_buffer || j > resolution - grid_buffer || i > resolution - grid_buffer) {
                newGrid[i][j] = 0;
            }
            else {
                int rand_res = rand() % 100;

                vector2 veroni_cell = find_shortest_distance(veroni_points, vector2{j, i});
                int veroni_center_distance = distance(vector2 {j, i}, veroni_cell);
                int veroni_dis_percent = 100 - ((veroni_center_distance / (float)(resolution)) * 100);
                int veroni_dis_percent_mod = (rand() % 200) - 100;

                int center_distance = distance(vector2{ j, i }, vector2{ resolution / 2, resolution / 2 });
                int center_dis_percent = 100 - ((center_distance / ((float)farthest_distance - (grid_buffer * 2))) * 100);

                if (center_dis_percent >= 100) {
                    newGrid[i][j] = 0;
                }
                else {
                    float land_threshold = (land_chance + (veroni_dis_percent - veroni_dis_percent_mod) + (center_dis_percent / archepeligo_val)) / 3;
                    if (rand_res <= land_threshold) {
                        newGrid[i][j] = 2;
                    }
                    else
                    {
                        newGrid[i][j] = 0;
                    }
                }
            }
        }
    }

    for (int i = 0; i < smoothing_steps; i++)
    {
        smooth(newGrid, resolution);
    }

    return newGrid;
}

int** bgTG::generateSimpleDotGrid(int resolution, int land_chance, int smoothing_steps)
{

    //the terrain to use
    int** newGrid = new int* [resolution];
    for (int i = 0; i < resolution; i++)
    {
        newGrid[i] = new int[resolution];
        for (int j = 0; j < resolution; j++)
        {
            if (j < 6 || i < 6 || j > resolution - 6 || i > resolution - 6) {
                newGrid[i][j] = 0;
            }
            else {
                int rand_res = rand() % 100;

                if (rand_res <= land_chance) {
                    newGrid[i][j] = 2;
                }
                else
                {
                    newGrid[i][j] = 0;
                }
            }
        }
    }

    for (int i = 0; i < smoothing_steps; i++)
    {
        smooth(newGrid, resolution);
    }

    return newGrid;
}

void bgTG::grow_continents(int** grid_to_grow, int resolution)
{
    for (int i = 0; i < resolution; i++)
    {
        for (int j = 0; j < resolution; j++)
        {
            int grow_chance = 25;
            if (grid_to_grow[i][j] >= 1) {
                if (i > 0) {
                    if (rand() % 100 < grow_chance) {
                        grid_to_grow[i - 1][j] = grid_to_grow[i][j];
                    }
                }
                if (j > 0) {
                    if (rand() % 100 < grow_chance) {
                        grid_to_grow[i][j - 1] = grid_to_grow[i][j];
                    }
                }
                if (i > 0 && j > 0) {
                    if (rand() % 100 < grow_chance) {
                        grid_to_grow[i - 1][j - 1] = grid_to_grow[i][j];
                    }
                }
                if (i > 0 && j < resolution - 1) {
                    if (rand() % 100 < grow_chance) {
                        grid_to_grow[i - 1][j + 1] = grid_to_grow[i][j];
                    }
                }
            }
        }
    }
}

void bgTG::grow_continents_reverse(int** grid_to_grow, int resolution)
{
    for (int i = resolution - 1; i > 0; i--)
    {
        for (int j = resolution - 1; j > 0; j--)
        {
            int grow_chance = 25;
            if (grid_to_grow[i][j] >= 1) {
                if (i < resolution - 1) {
                    if (rand() % 100 < grow_chance) {
                        grid_to_grow[i + 1][j] = 1;
                    }
                }
                if (j < resolution - 1) {
                    if (rand() % 100 < grow_chance) {
                        grid_to_grow[i][j + 1] = 1;
                    }
                }
                if (i < resolution - 1 && j < resolution - 1) {
                    if (rand() % 100 < grow_chance) {
                        grid_to_grow[i + 1][j + 1] = 1;
                    }
                }
                if (i < resolution - 1 && j > 0) {
                    if (rand() % 100 < grow_chance) {
                        grid_to_grow[i + 1][j - 1] = 1;
                    }
                }
            }
        }
    }
}

void generate_sand(int resolution, int** grid) {
    for (int i = 3; i < resolution - 3; i++)
    {
        for (int j = 3; j < resolution - 3; j++)
        {
            if (grid[i][j] == 0) {
                for (int k = -2; k <= 2; k++)
                {
                    for (int l = -2; l <= 2; l++)
                    {
                        if (k != 0 && l != 0) {
                            //if the grid square is grass then change to sand, otherwise keep the same
                            grid[i + k][j + l] = grid[i + k][j + l] == 2 ? 1 : grid[i + k][j + l];
                        }
                    }
                }
            }
        }
    }
}

int find_shortest_dir(int** grid, vector2 start) {
    int tile_count_x = 0;
    int tile_count_y = 0;
    int x = start.x;
    while (grid[x][start.y] != 0)
    {
        tile_count_x++;
        x++;
    }

    int y = start.y;
    while (grid[start.x][y] != 0)
    {
        tile_count_y++;
        y++;
    }

    return tile_count_x < tile_count_y ? 1 : -1;
}


int** bgTG::generate_continents(int grid_res, int continent_size, int river_amt, int mtn_amt)
{
    std::list<vector2> veroni_points = generate_worely_noise_grid(10, grid_res);
    int** newGrid = generateComplexDotGrid(grid_res, 50, 6, 3, veroni_points);
    //for (int i = 0; i < continent_size; i++)
    //{
    //    if (i % 2 == 1) {
    //        grow_continents(newGrid, grid_res);
    //    }
    //    else {
    //        grow_continents_reverse(newGrid, grid_res);
    //    }
    //}
    //smooth(newGrid, grid_res);

    for (int i = 0; i < mtn_amt; i++)
    {
        generate_mountain(grid_res, newGrid, 5, grid_res / 4, veroni_points);
    }
    for (int i = 0; i < river_amt; i++)
    {
        create_river(grid_res, newGrid);
    }
    generate_sand(grid_res, newGrid);
    return newGrid;
}

void bgTG::create_river(int grid_res, int** grid)
{
    //generate random start location within the inner quarter of map
    int tile_count = rand() % grid_res;
    int start_look_x = (rand() % (grid_res));
    int start_look_y = (rand() % (grid_res));
    int* start = new int[2]{ 0, 0 };
    //start on a land square
    for (int i = start_look_y; i < grid_res; i++)
    {
        for (int j = start_look_x; j < grid_res; j++)
        {
            if (grid[i][j] > 0)
            {
                start[0] = i;
                start[1] = j;
                i = grid_res;
                j = grid_res;
            }
        }
    }

    //Create a small circle of water to start, emulates bays/lakes
    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            if (start[0] + i < grid_res && start[1] + j < grid_res && start[0] + i > 0 && start[1] + j > 0) {
                grid[start[0] + i][start[1] + j] = 0;
            }
        }
    }
    if (start[0] > 1) {
        grid[start[0] - 2][start[1]] = 0;
    }
    if (start[0] < grid_res - 1) {
        grid[start[0] + 2][start[1]] = 0;
    }
    if (start[1] > 1) {
        grid[start[0]][start[1] + 2] = 0;
    }
    if (start[1] < grid_res - 1) {
        grid[start[0]][start[1] - 2] = 0;
    }

    int* cur_search_tile = start;
    //For keeping a more consistent direction
    int flow_direction = rand() % 8;
    int last_center_x = 1;
    int last_center_y = 1;
    //randomly select adjacent
    for (int i = 0; i < tile_count; i++)
    {
        int next_center_x = 0;
        int next_center_y = 0;

        int dir_mod = rand() % 2;
        if (dir_mod == 0)
            dir_mod = -1;
        double new_dir = (double)flow_direction + dir_mod;

        if (new_dir > 7)
            new_dir = 0;
        if (new_dir < 0)
            new_dir = 7;
        flow_direction = new_dir;

        int* new_coords = find_coords_on_square(new_dir);
        next_center_x = new_coords[0];
        next_center_y = new_coords[1];

        //ensure we don't just select the one we started on
        if (next_center_x == 0 && next_center_y == 0) {
            next_center_x = last_center_x;
            next_center_y = last_center_y;
        }

        last_center_x = next_center_x;
        last_center_y = next_center_y;

        cur_search_tile[0] = cur_search_tile[0] + next_center_y;
        cur_search_tile[1] = cur_search_tile[1] + next_center_x;


        if (rand() % (tile_count - i) > (tile_count * 9) / 10) {
            if (cur_search_tile[0] > 0 && cur_search_tile[1] > 0 && cur_search_tile[0] < grid_res - 1 && cur_search_tile[1] < grid_res - 1) {
                grid[cur_search_tile[0]][cur_search_tile[1]] = 0;
            }
        }
        else {
            for (int i = 0; i <= 1; i++)
            {
                for (int j = 0; j <= 1; j++)
                {
                    if (cur_search_tile[0] + i < grid_res - 1 && cur_search_tile[1] + j < grid_res - 1 && cur_search_tile[0] + i > 0 && cur_search_tile[1] + j > 0) {
                        grid[cur_search_tile[0] + i][cur_search_tile[1] + j] = 0;
                    }
                }
            }
        }
    }
}

void bgTG::generate_mountain(int grid_res, int** grid, int roughness, int max_length, std::list<vector2> veroni_points) {
    int x_length, y_length;

}
