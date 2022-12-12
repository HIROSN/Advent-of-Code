// #include <debug_print.h>
#include <main.h>

#include <climits>
#include <functional>
#include <map>
#include <queue>
#include <utility>
#include <vector>

using PointType = std::pair<int, int>;
using AdjacentPointsQueueType =
    std::priority_queue<std::pair<int, PointType>,
                        std::vector<std::pair<int, PointType>>,
                        std::greater<std::pair<int, PointType>>>;

#define X 0
#define Y 1

std::vector<PointType> adjacent_points(int x, int y);
int elevation(int x, int y);

#ifdef DPRINT_ON
void debug_print_path(const std::vector<std::vector<PointType>> &entered_from,
                      const PointType &start, const PointType &end);
#endif

std::vector<std::string> heightmap;
int size_x = 0;
int size_y = 0;

std::optional<uint64_t> Answer(std::ifstream &file)
{
    std::string line;
    PointType start, end;

    for (int y = 0; std::getline(file, line); y++)
    {
        int x = line.find("S");
        if (x != std::string::npos)
            start = std::make_pair(x, y);

        x = line.find("E");
        if (x != std::string::npos)
            end = std::make_pair(x, y);

        heightmap.push_back(line);
    }

    size_x = line.size();
    size_y = heightmap.size();
    DPRINT(heightmap);

    int sx = std::get<X>(start);
    int sy = std::get<Y>(start);

    std::vector<std::vector<int>> steps;
    std::vector<std::vector<bool>> entered;
#ifdef DPRINT_ON
    std::vector<std::vector<PointType>> entered_from;
#endif

    for (int y = 0; y < size_y; y++)
    {
        steps.emplace_back(size_x, INT_MAX);
        entered.emplace_back(size_x, false);
#ifdef DPRINT_ON
        entered_from.emplace_back(size_x, PointType());
#endif
    }

    AdjacentPointsQueueType entered_points;
    std::map<PointType, bool> queued;
    PointType point = start;
    steps[sy][sx] = 0;
    entered[sy][sx] = true;

    while (true)
    {
        int x = std::get<X>(point);
        int y = std::get<Y>(point);

        for (auto &adjacent : adjacent_points(x, y))
        {
            int ax = std::get<X>(adjacent);
            int ay = std::get<Y>(adjacent);

            if (!entered[ay][ax])
            {
                if (steps[ay][ax] > steps[y][x] + 1)
                {
                    steps[ay][ax] = steps[y][x] + 1;
#ifdef DPRINT_ON
                    entered_from[ay][ax] = point;
#endif
                }

                if (queued.find(adjacent) == queued.end())
                {
                    entered_points.push({steps[ay][ax], adjacent});
                    queued[adjacent] = true;
                }
            }
        }

        if (!entered_points.empty())
        {
            point = entered_points.top().second;
            entered_points.pop();
            queued.erase(point);
            entered[y][x] = true;
        }
        else
        {
            break;
        }
    }

#ifdef DPRINT_ON
    debug_print_path(entered_from, start, end);
#endif
    int fewest_steps = steps[std::get<Y>(end)][std::get<X>(end)];
    if (fewest_steps)
        return fewest_steps;
    return {};
}

std::vector<PointType> adjacent_points(int x, int y)
{
    static const PointType offset[] = {
        {0, -1},
        {-1, 0},
        {1, 0},
        {0, 1},
    };

    int adjacent_size = sizeof(offset) / sizeof(offset[0]);
    std::vector<PointType> adjacent;

    for (int i = 0; i < adjacent_size; i++)
    {
        int ax = x + std::get<X>(offset[i]);
        int ay = y + std::get<Y>(offset[i]);

        if (ay >= 0 && ay < size_y &&
            ax >= 0 && ax < size_x &&
            !(ax == x && ay == y) &&
            elevation(ax, ay) - elevation(x, y) <= 1)
        {
            adjacent.push_back(std::make_pair(ax, ay));
        }
    }

    return adjacent;
}

int elevation(int x, int y)
{
    char height = heightmap[y][x];

    switch (height)
    {
    case 'S':
        height = 'a';
        break;
    case 'E':
        height = 'z';
        break;
    }

    return height - 'a';
}

#ifdef DPRINT_ON
void debug_print_path(const std::vector<std::vector<PointType>> &entered_from,
                      const PointType &start, const PointType &end)
{
    PointType point = end;
    std::vector<std::string> heightmap_with_path;
    for (int i = 0; i < size_y; i++)
        heightmap_with_path.push_back(std::string(size_x, '.'));

    while (true)
    {
        int x = std::get<X>(point);
        int y = std::get<Y>(point);
        const auto &from = entered_from[y][x];

        if (point != start)
        {
            if (point == end)
                heightmap_with_path[y][x] = 'E';

            int from_x = std::get<X>(from);
            int from_y = std::get<Y>(from);

            if (from_x < x)
                heightmap_with_path[from_y][from_x] = '>';
            else if (from_x > x)
                heightmap_with_path[from_y][from_x] = '<';
            else if (from_y < y)
                heightmap_with_path[from_y][from_x] = 'v';
            else if (from_y > y)
                heightmap_with_path[from_y][from_x] = '^';

            point = from;
        }
        else
        {
            break;
        }
    }

    DPRINT(heightmap_with_path);
}
#endif
