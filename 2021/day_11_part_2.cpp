#include "main.h"

#include <algorithm>
#include <utility>
#include <vector>

using PointType = std::pair<int, int>;

#define X 0
#define Y 1

std::vector<PointType> adjacent_points(const PointType &point);
std::vector<PointType> adjacent_points(int x, int y);
int energy_at(const PointType &point);
int energy_at(int x, int y);
void set_energy_at(const PointType &point, int energy);
void do_step(const PointType &point, std::vector<PointType> &flashes);

std::vector<std::string> energymap;
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
            energymap.push_back(line);
        }
    }

    size_y = energymap.size();
    const int all_octopuses = size_x * size_y;
    int step = 0;

    for (step = 1; true; step++)
    {
        std::vector<PointType> flashes;

        for (int y = 0; y < size_y; y++)
        {
            for (int x = 0; x < size_x; x++)
            {
                do_step(std::make_pair(x, y), flashes);
            }
        }

        if (flashes.size() == all_octopuses)
        {
            break;
        }
    }

    return step;
}

std::vector<PointType> adjacent_points(const PointType &point)
{
    return adjacent_points(std::get<X>(point), std::get<Y>(point));
}

std::vector<PointType> adjacent_points(int x, int y)
{
    static const PointType offset[] = {
        {-1, -1},
        {0, -1},
        {1, -1},
        {1, 0},
        {1, 1},
        {0, 1},
        {-1, 1},
        {-1, 0},
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

int energy_at(const PointType &point)
{
    return energy_at(std::get<X>(point), std::get<Y>(point));
}

int energy_at(int x, int y)
{
    return energymap[y][x] - '0';
}

void set_energy_at(const PointType &point, int energy)
{
    energymap[std::get<Y>(point)][std::get<X>(point)] = '0' + energy;
}

void do_step(const PointType &point, std::vector<PointType> &flashes)
{
    if (flashes.size() > 0)
    {
        if (std::find(flashes.begin(), flashes.end(), point) != flashes.end())
        {
            return;
        }
    }

    int energy = energy_at(point) + 1;

    if (energy > 9)
    {
        flashes.push_back(point);
        set_energy_at(point, 0);

        for (auto &p : adjacent_points(point))
        {
            do_step(p, flashes);
        }
    }
    else
    {
        set_energy_at(point, energy);
    }
}
