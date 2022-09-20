#include "main.h"

#include <algorithm>
#include <map>
#include <utility>

int Answer(std::ifstream &file)
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
            if (x1 == x2)
            {
                for (int y = std::min(y1, y2); y <= std::max(y1, y2); y++)
                {
                    if (++points[std::make_pair(x1, y)] == 2)
                    {
                        overlaps++;
                    }
                }
            }
            else if (y1 == y2)
            {
                for (int x = std::min(x1, x2); x <= std::max(x1, x2); x++)
                {
                    if (++points[std::make_pair(x, y1)] == 2)
                    {
                        overlaps++;
                    }
                }
            }
        }
    }

    return overlaps;
}
