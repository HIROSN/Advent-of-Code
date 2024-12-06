// #include <debug_print.h>
#include <main.h>

#include <vector>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    std::vector<std::string> map;
    std::string line;

    struct Point
    {
        int x;
        int y;
    };

    Point guard;

    while (std::getline(file, line))
    {
        int x = line.find('^');

        if (x != std::string::npos)
        {
            guard.x = x;
            guard.y = map.size();
        }

        map.push_back(line);
    }

    DPRINT(map);
    const int size_x = line.size();
    const int size_y = map.size();
    const Point step[4] = {{0, -1}, {1, 0}, {0, 1}, {-1, 0}};
    int facing = 0;
    int distinct_positions = 0;

    while (guard.x >= 0 && guard.x < size_x &&
           guard.y >= 0 && guard.y < size_y)
    {
        if (map[guard.y][guard.x] != 'X')
        {
            map[guard.y][guard.x] = 'X';
            distinct_positions++;
        }

        int nx = guard.x + step[facing].x;
        int ny = guard.y + step[facing].y;

        if (map[ny][nx] == '#')
        {
            facing = ++facing % 4;
        }
        else
        {
            guard.x = nx;
            guard.y = ny;
        }
    }

    DPRINT(map);
    return distinct_positions;
}
