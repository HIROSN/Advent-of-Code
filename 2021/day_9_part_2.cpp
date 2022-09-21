#include "main.h"

#include <algorithm>
#include <functional>
#include <queue>
#include <utility>
#include <vector>

using reverse_priority_queue = std::priority_queue<int, std::vector<int>,
                                                   std::greater<int>>;
using PointType = std::pair<int, int>;

#define X 0
#define Y 1

std::vector<PointType> adjacent_points(const PointType &point);
std::vector<PointType> adjacent_points(int x, int y);
int height_at(const PointType &point);
int height_at(int x, int y);
void find_basin(const PointType &point, std::vector<PointType> &basin);

std::vector<std::string> heightmap;
int size_x = 0;
int size_y = 0;

uint64_t Answer(std::ifstream &file)
{
    std::string line;

    while (file >> line)
    {
        if (size_x == 0)
        {
            size_x = line.size();
        }

        if (line.size() == size_x)
        {
            heightmap.push_back(line);
        }
    }

    size_y = heightmap.size();
    std::priority_queue<int> largest_basin_sizes;

    for (int y = 0; y < size_y; y++)
    {
        for (int x = 0; x < size_x; x++)
        {
            reverse_priority_queue adjacent_low;

            for (auto &p : adjacent_points(x, y))
            {
                adjacent_low.push(height_at(p));
            }

            if (height_at(x, y) < adjacent_low.top())
            {
                std::vector<PointType> basin;
                find_basin(std::make_pair(x, y), basin);
                largest_basin_sizes.push(basin.size());
            }
        }
    }

    int multiply_basin_sizes = 1;

    for (int i = 0; i < 3 && !largest_basin_sizes.empty(); i++)
    {
        multiply_basin_sizes *= largest_basin_sizes.top();
        largest_basin_sizes.pop();
    }

    return multiply_basin_sizes;
}

std::vector<PointType> adjacent_points(const PointType &point)
{
    return adjacent_points(std::get<X>(point), std::get<Y>(point));
}

std::vector<PointType> adjacent_points(int x, int y)
{
    static const PointType offset[] = {
        {0, -1},
        {-1, 0},
        {1, 0},
        {0, 1},
    };

    int adjacent_size = sizeof(offset) / sizeof(offset[0]);
    std::vector<PointType> adjacent;

    for (int i = 0; i < adjacent_size; i++)
    {
        int ax = x + std::get<X>(offset[i]);
        int ay = y + std::get<Y>(offset[i]);

        if (ay >= 0 && ay < size_y &&
            ax >= 0 && ax < size_x)
        {
            adjacent.push_back(std::make_pair(ax, ay));
        }
    }

    return adjacent;
}

int height_at(const PointType &point)
{
    return height_at(std::get<X>(point), std::get<Y>(point));
}

int height_at(int x, int y)
{
    return heightmap[y][x] - '0';
}

void find_basin(const PointType &point, std::vector<PointType> &basin)
{
    int height = height_at(point);

    if (height == 9)
    {
        return;
    }

    if (basin.size() > 0)
    {
        if (std::find(basin.begin(), basin.end(), point) != basin.end())
        {
            return;
        }
    }

    basin.push_back(point);

    for (auto &p : adjacent_points(point))
    {
        if (height_at(p) > height)
        {
            find_basin(p, basin);
        }
    }
}
