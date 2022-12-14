// #include <debug_print.h>
#include <main.h>
#include <skip.h>

#include <algorithm>
#include <map>
#include <sstream>
#include <utility>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    using Point = std::pair<int, int>;
    std::map<Point, char> map;
    const Point source = {500, 0};
    map[source] = '+';
    Point top_left = source;
    Point bottom_right = source;
    std::string line;

    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        Point start, end;
        char comma;
        ss >> start.first >> comma >> start.second;
        while (skip<std::string>(ss))
        {
            ss >> end.first >> comma >> end.second;
            for (int x = std::min(start.first, end.first);
                 x <= std::max(start.first, end.first);
                 x++)
            {
                for (int y = std::min(start.second, end.second);
                     y <= std::max(start.second, end.second);
                     y++)
                {
                    map[std::make_pair(x, y)] = '#';
                    top_left.first = std::min(top_left.first, x);
                    top_left.second = std::min(top_left.second, y);
                    bottom_right.first = std::max(bottom_right.first, x);
                    bottom_right.second = std::max(bottom_right.second, y);
                }
            }
            start = end;
        }
    }
    DPRINT3(map, '.', 1);
    DPRINT(top_left);
    DPRINT(bottom_right);
    DPRINT_ENDL();

    int units_of_sand = 0;
    map.erase(source);
    while (map.find(source) == map.end())
    {
        Point sand = source;
        while (true)
        {
            if (sand.second == bottom_right.second + 1)
                break;

            Point down = {sand.first, sand.second + 1};
            Point down_left = {sand.first - 1, sand.second + 1};
            Point down_right = {sand.first + 1, sand.second + 1};
            if (map.find(down) == map.end())
            {
                sand = down;
                continue;
            }
            if (map.find(down_left) == map.end())
            {
                sand = down_left;
                continue;
            }
            if (map.find(down_right) == map.end())
            {
                sand = down_right;
                continue;
            }
            break;
        }

        map[sand] = 'o';
        units_of_sand++;
    }
    DPRINT3(map, '.', 1);

    return units_of_sand;
}
