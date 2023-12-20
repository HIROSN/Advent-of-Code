// #include <debug_print.h>
#define PART_1
#include <main.h>

#include <algorithm>
#include <sstream>
#include <vector>

struct Point
{
    int x = 0;
    int y = 0;
};

using Points = std::vector<Point>;

int part_1_count_interiors(std::vector<std::string> grid)
{
    int interiors = 0;

    const int grid_size_x = grid[0].size();
    const int grid_size_y = grid.size();

    const Points forward{{1, 0}, {1, 1}, {0, 1}};
    const Points backward{{-1, 0}, {-1, -1}, {0, -1}};

    auto find_interior = [&](const Points &adjacent, int x, int y) -> void
    {
        const char &ch = grid[y][x];
        if (ch != '.')
        {
            if (!interiors || ch == 'I')
            {
                for (const auto &adj : adjacent)
                {
                    const int ax = x + adj.x;
                    const int ay = y + adj.y;
                    char &ach = grid[ay][ax];

                    if (ach == '.')
                    {
                        ach = 'I';
                        interiors++;
                    }
                }
            }
        }
    };

    while (true)
    {
        const auto not_changed = interiors;

        for (int y = 0; y < grid_size_y - 1; y++)
            for (int x = 0; x < grid_size_x - 1; x++)
                find_interior(forward, x, y);

        for (int y = grid_size_y - 1; y > 0; y--)
            for (int x = grid_size_x - 1; x > 0; x--)
                find_interior(backward, x, y);

        if (interiors == not_changed)
            break;
    }

    DPRINT(grid);
    return interiors;
}

uint64_t divide_and_conquer(const Points &points,
                            const Point &block_start, int block_size,
                            int size_x, int size_y)
{
    const int max_grid_size = 140;

    block_size = block_size / max_grid_size + 1;
    int grid_size_x = size_x / block_size + (size_x % block_size ? 1 : 0);
    int grid_size_y = size_y / block_size + (size_y % block_size ? 1 : 0);

    // DPRINT2(block_size, points.size());
    // DPRINT2(grid_size_x, grid_size_y);
    // DPRINT_ENDL();

    std::vector<std::vector<Points>> grid_of_points(
        grid_size_y, std::vector<Points>(grid_size_x, Points()));

    std::vector<std::string> grid(
        grid_size_y, std::string(grid_size_x, '.'));

    auto bx = [&](int x) -> int
    {
        return (x - block_start.x) / block_size;
    };

    auto by = [&](int y) -> int
    {
        return (y - block_start.y) / block_size;
    };

    for (const auto &point : points)
    {
        grid_of_points[by(point.y)][bx(point.x)].push_back(point);
        grid[by(point.y)][bx(point.x)] = '#';
    }

    uint64_t interiors = 0;

    if (block_size == 1)
    {
        // DPRINT(grid);
    }
    else
    {
        for (int by = 0; by < grid_size_y; by++)
        {
            for (int bx = 0; bx < grid_size_x; bx++)
            {
                if (grid[by][bx] == '#')
                {
                    auto new_block_start = block_start;
                    new_block_start.x += block_size * bx;
                    new_block_start.y += block_size * by;

                    int new_size_x = std::min(
                        size_x - block_size * bx, block_size);
                    int new_size_y = std::min(
                        size_y - block_size * by, block_size);

                    interiors += divide_and_conquer(
                        grid_of_points[by][bx],
                        new_block_start, block_size,
                        new_size_x, new_size_y);
                }
            }
        }
    }

    return interiors;
}

std::optional<uint64_t> Answer(std::ifstream &file)
{
    struct Lagoon
    {
        Points points;

        size_t cubic_meters() const
        {
            return points.size();
        }
    };

    Lagoon lagoon;
    Point digger, start, end;
    char direction;
    int number;
    std::string line;

#ifdef PART_1
    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string str;
        int color;
        ss >> direction >> number >> str;
        std::stringstream(str.substr(2, 6)) >> std::hex >> color;

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
#else
    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string str;
        ss >> direction >> number >> str;
        std::stringstream(str.substr(2, 5)) >> std::hex >> number;
        direction = str[7];

        for (int i = 0; i < number; i++)
        {
            switch (direction)
            {
            case '0':
                digger.x++;
                break;
            case '1':
                digger.y++;
                break;
            case '2':
                digger.x--;
                break;
            case '3':
                digger.y--;
                break;
            }

            lagoon.points.push_back(digger);
            start.x = std::min(start.x, digger.x);
            start.y = std::min(start.y, digger.y);
            end.x = std::max(end.x, digger.x);
            end.y = std::max(end.y, digger.y);
        }
    }
#endif
    const int size_x = end.x - start.x + 1;
    const int size_y = end.y - start.y + 1;

    DPRINT2(start.x, start.y);
    DPRINT2(end.x, end.y);
    DPRINT2(size_x, size_y);
    DPRINT_ENDL();

    uint64_t cubic_meters = lagoon.cubic_meters();

#ifdef PART_1
    std::vector<std::string> grid(
        size_y, std::string(size_x, '.'));

    for (const auto &point : lagoon.points)
        grid[point.y - start.y][point.x - start.x] = '#';

    DPRINT(grid);
    cubic_meters += part_1_count_interiors(grid);
#else
    cubic_meters += divide_and_conquer(
        lagoon.points, start, size_x, size_x, size_y);
#endif
    return cubic_meters;
}
