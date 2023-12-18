// #include <debug_print.h>
#include <main.h>

#include <algorithm>
#include <sstream>
#include <vector>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    struct Point
    {
        int x = 0;
        int y = 0;
        int color = 0;
    };

    struct Lagoon
    {
        std::vector<Point> points;
        std::vector<std::string> grid;

        int cubic_meters() const
        {
            return points.size();
        }
    };

    Lagoon lagoon;
    Point digger, start, end;
    char direction;
    int number;
    std::string line;

    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string str;
        ss >> direction >> number >> str;
        std::stringstream(str.substr(2, 6)) >> std::hex >> digger.color;

        for (int i = 0; i < number; i++)
        {
            switch (direction)
            {
            case 'U':
                digger.y--;
                break;
            case 'D':
                digger.y++;
                break;
            case 'L':
                digger.x--;
                break;
            case 'R':
                digger.x++;
                break;
            }

            lagoon.points.push_back(digger);
            start.x = std::min(start.x, digger.x);
            start.y = std::min(start.y, digger.y);
            end.x = std::max(end.x, digger.x);
            end.y = std::max(end.y, digger.y);
        }
    }

    const int size_x = end.x - start.x + 1;
    const int size_y = end.y - start.y + 1;

    DPRINT2(start.x, start.y);
    DPRINT2(end.x, end.y);
    DPRINT2(size_x, size_y);
    DPRINT_ENDL();

    lagoon.grid = std::vector<std::string>(
        size_y, std::string(size_x, '.'));

    for (const auto &point : lagoon.points)
        lagoon.grid[point.y - start.y][point.x - start.x] = '#';

    DPRINT(lagoon.grid);
    int cubic_meters = lagoon.cubic_meters();
    bool has_interior = false;

    const Point forward[3] = {{1, 0}, {1, 1}, {0, 1}};
    const Point backward[3] = {{-1, 0}, {-1, -1}, {0, -1}};

    while (true)
    {
        for (int y = 0; y < size_y - 1; y++)
        {
            for (int x = 0; x < size_x - 1; x++)
            {
                const char &ch = lagoon.grid[y][x];
                if (ch != '.')
                {
                    if (!has_interior || ch == 'I')
                    {
                        for (const auto &adj : forward)
                        {
                            const int ax = x + adj.x;
                            const int ay = y + adj.y;
                            char &ach = lagoon.grid[ay][ax];

                            if (ach == '.')
                            {
                                ach = 'I';
                                lagoon.points.push_back(
                                    {start.x + ax, start.y + ay});
                                has_interior = true;
                            }
                        }
                    }
                }
            }
        }

        for (int y = size_y - 1; y > 0; y--)
        {
            for (int x = size_x - 1; x > 0; x--)
            {
                const char &ch = lagoon.grid[y][x];
                if (ch != '.')
                {
                    if (!has_interior || ch == 'I')
                    {
                        for (const auto &adj : backward)
                        {
                            const int ax = x + adj.x;
                            const int ay = y + adj.y;
                            char &ach = lagoon.grid[ay][ax];

                            if (ach == '.')
                            {
                                ach = 'I';
                                lagoon.points.push_back(
                                    {start.x + ax, start.y + ay});
                            }
                        }
                    }
                }
            }
        }

        if (cubic_meters == lagoon.cubic_meters())
            break;

        cubic_meters = lagoon.cubic_meters();
    }

    DPRINT(lagoon.grid);
    return cubic_meters;
}
