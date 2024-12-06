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

    auto patrol = [size_x, size_y, step](std::vector<std::string> map, Point guard) -> bool
    {
        std::vector<std::vector<int>> visited(size_y, std::vector<int>(size_x, 0));
        int facing = 0;

        while (guard.x >= 0 && guard.x < size_x &&
               guard.y >= 0 && guard.y < size_y)
        {
            if (visited[guard.y][guard.x]++ == 4)
                return false;

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

        return true;
    };

    int obstruction_positions = 0;

    for (int y = 0; y < size_y; y++)
    {
        for (int x = 0; x < size_x; x++)
        {
            if (map[y][x] == '#')
                continue;

            auto map_copy = map;
            map_copy[y][x] = '#';
            if (!patrol(map_copy, guard))
            {
                obstruction_positions++;
                map_copy[y][x] = 'O';
                DPRINT(map_copy);
            }
        }
    }

    return obstruction_positions;
}
