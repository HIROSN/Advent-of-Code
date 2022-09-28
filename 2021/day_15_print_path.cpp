#include "debug_print.h"
#include "main.h"

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
int risk_at(int x, int y);

#ifdef DPRINT_ON
void debug_print_path(std::vector<std::string> cavern_with_path,
                      const std::vector<std::vector<PointType>> &entered_from);
#endif

std::vector<std::string> cavern;
int size_x = 0;
int size_y = 0;

uint64_t Answer(std::ifstream &file)
{
    std::string line;

    while (file >> line)
    {
        if (size_x == 0)
        {
            size_x = line.size();
        }

        if (line.size() == size_x)
        {
            cavern.push_back(line);
        }
    }

    size_y = cavern.size();

    std::vector<std::vector<int>> risks;
    std::vector<std::vector<bool>> entered;
#ifdef DPRINT_ON
    std::vector<std::vector<PointType>> entered_from;
#endif

    for (int y = 0; y < size_y; y++)
    {
        risks.emplace_back(size_x, INT_MAX);
        entered.emplace_back(size_x, false);
#ifdef DPRINT_ON
        entered_from.emplace_back(size_x, PointType());
#endif
    }

    AdjacentPointsQueueType entered_points;
    std::map<PointType, bool> queued;
    PointType point{0, 0};
    risks[0][0] = 0;
    entered[0][0] = true;

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
                if (risks[ay][ax] > risks[y][x] + risk_at(ax, ay))
                {
                    risks[ay][ax] = risks[y][x] + risk_at(ax, ay);
#ifdef DPRINT_ON
                    entered_from[ay][ax] = point;
#endif
                }

                if (queued.find(adjacent) == queued.end())
                {
                    entered_points.push({risks[ay][ax], adjacent});
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
    debug_print_path(cavern, entered_from);
#endif

    return risks[size_y - 1][size_x - 1];
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
            !(ax == x && ay == y))
        {
            adjacent.push_back(std::make_pair(ax, ay));
        }
    }

    return adjacent;
}

int risk_at(int x, int y)
{
    return cavern[y][x] - '0';
}

#ifdef DPRINT_ON
void debug_print_path(std::vector<std::string> cavern_with_path,
                      const std::vector<std::vector<PointType>> &entered_from)
{
    PointType point{size_x - 1, size_y - 1};

    for (int i = 0; i < size_x * size_y; i++)
    {
        int x = std::get<X>(point);
        int y = std::get<Y>(point);
        cavern_with_path[y][x] = ' ';

        if (x > 0 || y > 0)
        {
            point = entered_from[y][x];
        }
        else
        {
            break;
        }
    }

    DPRINT(cavern_with_path);
}
#endif
