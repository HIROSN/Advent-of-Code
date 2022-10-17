// #include "debug_print.h"
#include "main.h"

#include <utility>
#include <vector>

int size_east = 0;
int size_south = 0;

using SeaFloor = std::vector<std::string>;

SeaFloor read_seafloor(std::ifstream &file);

uint64_t Answer(std::ifstream &file)
{
    auto prev_seafloor = read_seafloor(file);
    DPRINT(prev_seafloor);
    int step = 0;

    while (++step)
    {
        bool moved_at_all = false;
        auto seafloor = prev_seafloor;

        for (int s = 0; s < size_south; s++)
        {
            for (int e0 = 0; e0 < size_east; e0++)
            {
                const int e1 = (e0 + 1) % size_east;

                if (prev_seafloor[s][e0] == '>' &&
                    prev_seafloor[s][e1] == '.')
                {
                    std::swap(seafloor[s][e0], seafloor[s][e1]);
                    moved_at_all = true;
                }
            }
        }

        prev_seafloor = seafloor;

        for (int e = 0; e < size_east; e++)
        {
            for (int s0 = 0; s0 < size_south; s0++)
            {
                const int s1 = (s0 + 1) % size_south;

                if (prev_seafloor[s0][e] == 'v' &&
                    prev_seafloor[s1][e] == '.')
                {
                    std::swap(seafloor[s0][e], seafloor[s1][e]);
                    moved_at_all = true;
                }
            }
        }

        DPRINT(seafloor);
        prev_seafloor = std::move(seafloor);

        if (!moved_at_all)
        {
            break;
        }
    }

    return step;
}

SeaFloor read_seafloor(std::ifstream &file)
{
    SeaFloor seafloor;
    std::string line;

    while (std::getline(file, line))
    {
        if (!size_east)
        {
            size_east = line.size();
        }

        seafloor.push_back(line);
    }

    size_south = seafloor.size();
    return seafloor;
}
