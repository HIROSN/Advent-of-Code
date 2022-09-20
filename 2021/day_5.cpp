#include "main.h"

#include <algorithm>
#include <cmath>
#include <map>
#include <utility>

uint64_t Answer(std::ifstream &file)
{
    std::string start, arrow, end;
    int x1, y1, x2, y2;
    std::map<std::pair<int, int>, int> points;
    int overlaps = 0;

    while (file >> start >> arrow >> end)
    {
        if (arrow == "->" &&
            sscanf(start.c_str(), "%d,%d", &x1, &y1) == 2 &&
            sscanf(end.c_str(), "%d,%d", &x2, &y2) == 2)
        {
            int dx = std::abs(x2 - x1);
            int dy = std::abs(y2 - y1);

            if (dx == 0 || dy == 0)
            {
                int steps = std::max(dx, dy);
                int sx = dx ? (x2 - x1) / dx : 0;
                int sy = dy ? (y2 - y1) / dy : 0;

                for (int i = 0; i <= steps; i++)
                {
                    if (++points[std::make_pair(x1 + sx * i,
                                                y1 + sy * i)] == 2)
                    {
                        overlaps++;
                    }
                }
            }
        }
    }

    return overlaps;
}
