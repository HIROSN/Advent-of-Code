#include "main.h"

#include <algorithm>
#include <cmath>
#include <functional>
#include <sstream>
#include <tuple>
#include <unordered_set>

using Coordinate = std::tuple<int, int, int>;

namespace std
{
    template <>
    struct hash<Coordinate>
    {
        const size_t operator()(const Coordinate &c) const
        {
            std::stringstream ss;
            ss << std::get<0>(c) << ",";
            ss << std::get<1>(c) << ",";
            ss << std::get<2>(c);
            return std::hash<std::string>()(ss.str());
        }
    };
}

uint64_t Answer(std::ifstream &file)
{
    std::string line;
    std::unordered_set<Coordinate> cubes_on;

    while (std::getline(file, line))
    {
        char turn[3 + 1] = {};
        int x1, x2, y1, y2, z1, z2;

        if (sscanf(line.c_str(),
                   "%s x=%d..%d,y=%d..%d,z=%d..%d",
                   turn, &x1, &x2, &y1, &y2, &z1, &z2) == 7)
        {
            if (std::abs(x1) > 50 || std::abs(x2) > 50 ||
                std::abs(y1) > 50 || std::abs(y2) > 50 ||
                std::abs(z1) > 50 || std::abs(z2) > 50)
            {
                continue;
            }

            int xs = std::min(x1, x2);
            int xe = xs + std::abs(x1 - x2) + 1;
            int ys = std::min(y1, y2);
            int ye = ys + std::abs(y1 - y2) + 1;
            int zs = std::min(z1, z2);
            int ze = zs + std::abs(z1 - z2) + 1;

            for (int x = xs; x < xe; x++)
            {
                for (int y = ys; y < ye; y++)
                {
                    for (int z = zs; z < ze; z++)
                    {
                        Coordinate c{x, y, z};
                        auto it = cubes_on.find(c);
                        static const std::string on("on");

                        if (turn == on)
                        {
                            if (it == cubes_on.end())
                            {
                                cubes_on.insert(c);
                            }
                        }
                        else
                        {
                            if (it != cubes_on.end())
                            {
                                cubes_on.erase(it);
                            }
                        }
                    }
                }
            }
        }
    }

    return cubes_on.size();
}
