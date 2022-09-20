#include "main.h"

#include <cmath>
#include <map>
#include <queue>
#include <sstream>
#include <vector>

std::vector<int> crab;

int TotalCost(int position_to_align);
int Cost(int step);

uint64_t Answer(std::ifstream &file)
{
    int cost = 0;
    std::priority_queue<int> q;

    while (file.good())
    {
        std::string str;
        std::getline(file, str, ',');
        int position;
        std::stringstream(str) >> position;
        q.push(position);
    }

    while (!q.empty())
    {
        crab.push_back(q.top());
        q.pop();
    }

    if (crab.size() > 1)
    {
        int high = crab[0];
        int low = crab[crab.size() - 1];

        while (high > low)
        {
            int center = (high + low) / 2;
            int cost_left = TotalCost(center + 1);
            int cost_right = TotalCost(center);

            if (cost_left < cost_right)
            {
                cost = cost_left;
                low = center + 1;
            }
            else
            {
                cost = cost_right;
                high = center;
            }
        }
    }

    return cost;
}

int TotalCost(int position_to_align)
{
    int cost = 0;
    static std::map<int, int> cost_cache;

    if (cost_cache.find(position_to_align) != cost_cache.end())
    {
        return cost_cache[position_to_align];
    }

    for (auto position : crab)
    {
        cost += Cost(std::abs(position_to_align - position));
    }

    cost_cache[position_to_align] = cost;
    return cost;
}

int Cost(int step)
{
    if (step <= 0)
    {
        return 0;
    }

    static std::map<int, int> cost_cache;

    if (cost_cache.find(step) != cost_cache.end())
    {
        return cost_cache[step];
    }

    int cost = Cost(step - 1) + step;
    cost_cache[step] = cost;

    return cost;
}
