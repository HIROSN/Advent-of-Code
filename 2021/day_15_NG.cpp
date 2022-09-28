#include "main.h"

#include <functional>
#include <queue>
#include <utility>
#include <vector>

#define X 0
#define Y 1

using PointType = std::pair<int, int>;
using LowestRiskPointsType =
    std::priority_queue<PointType, std::vector<PointType>,
                        std::function<bool(PointType, PointType)>>;
using CavernType = std::vector<std::vector<int>>;

struct PathInfo
{
    int total_risk = 0;
    int end_x = 0;
    int end_y = 0;
    std::vector<PointType> entered;

    PathInfo() = default;
    PathInfo(int x, int y) : end_x(x), end_y(y) {}
};

CavernType cavern;
int size_x = 0;
int size_y = 0;
PathInfo lowest_total_risk_path;

PathInfo find_lowest_risk_path(const PointType &start,
                               const PointType &end);
void enter(int x, int y, PathInfo info);
LowestRiskPointsType adjacent_points(const PointType &point,
                                     const PathInfo &info);
bool compare(PointType left, PointType right);
int risk_at(int x, int y);

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
            std::vector<int> row;

            for (auto &ch : line)
            {
                row.push_back(ch - '0');
            }

            cavern.push_back(std::move(row));
        }
    }

    size_y = cavern.size();
    auto start = std::make_pair(0, 0);
    auto end = std::make_pair(size_x - 1, size_y - 1);
    auto path_info = find_lowest_risk_path(start, end);

    return path_info.total_risk;
}

PathInfo find_lowest_risk_path(const PointType &start,
                               const PointType &end)
{
    lowest_total_risk_path.total_risk = size_x * size_y * 9;
    auto adjacent = adjacent_points(start, PathInfo());

    while (!adjacent.empty())
    {
        auto &point = adjacent.top();
        enter(std::get<X>(point), std::get<Y>(point),
              PathInfo(std::get<X>(end), std::get<Y>(end)));
        adjacent.pop();
    }

    return lowest_total_risk_path;
}

void enter(int x, int y, PathInfo info)
{
    info.total_risk += risk_at(x, y);
    auto entered = std::make_pair(x, y);
    info.entered.push_back(entered);

    if (x == info.end_x && y == info.end_y)
    {
        if (info.total_risk < lowest_total_risk_path.total_risk)
        {
            lowest_total_risk_path = info;
        }
        return;
    }

    auto adjacent = adjacent_points(entered, info);

    while (!adjacent.empty())
    {
        auto &point = adjacent.top();
        enter(std::get<X>(point), std::get<Y>(point), info);
        adjacent.pop();
    }
}

LowestRiskPointsType adjacent_points(const PointType &point,
                                     const PathInfo &info)
{
    static const PointType offset[] = {
        {0, -1},
        {-1, 0},
        {1, 0},
        {0, 1},
    };

    int adjacent_size = sizeof(offset) / sizeof(offset[0]);
    LowestRiskPointsType adjacent(compare);

    for (int i = 0; i < adjacent_size; i++)
    {
        int ax = std::get<X>(point) + std::get<X>(offset[i]);
        int ay = std::get<Y>(point) + std::get<Y>(offset[i]);

        if (ax >= 0 && ax < size_x && ay >= 0 && ay < size_y)
        {
            int risk = risk_at(ax, ay);

            if (risk >= 1 && risk <= 9 &&
                info.total_risk + risk < lowest_total_risk_path.total_risk)
            {
                auto ap = std::make_pair(ax, ay);

                if (std::find(info.entered.begin(),
                              info.entered.end(), ap) == info.entered.end())
                {
                    adjacent.push(ap);
                }
            }
        }
    }

    return adjacent;
}

bool compare(PointType left, PointType right)
{
    return cavern[std::get<Y>(left)][std::get<X>(left)] >
           cavern[std::get<Y>(right)][std::get<X>(right)];
}

int risk_at(int x, int y)
{
    return cavern[y][x];
}
