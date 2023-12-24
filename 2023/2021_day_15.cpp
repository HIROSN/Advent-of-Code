// #include <debug_print.h>
#include <main.h>
#include <traveler.h>

#include <climits>
#include <functional>
#include <map>
#include <queue>
#include <utility>
#include <vector>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    std::vector<std::string> cavern;
    std::string line;

    while (file >> line)
        cavern.push_back(line);

    int size_x = line.size();
    int size_y = cavern.size();
    DPRINT(cavern);

    std::vector<std::vector<int>> risks;
    std::vector<std::vector<bool>> entered;

    for (int y = 0; y < size_y; y++)
    {
        risks.emplace_back(size_x, INT_MAX);
        entered.emplace_back(size_x, false);
    }

    using AdjacentPointsQueueType =
        std::priority_queue<std::pair<int, aoc::Traveler>,
                            std::vector<std::pair<int, aoc::Traveler>>,
                            std::greater<std::pair<int, aoc::Traveler>>>;

    AdjacentPointsQueueType entered_points;
    std::map<aoc::Traveler, bool> queued;
    aoc::Traveler traveler({0, 0}, {size_x - 1, size_y - 1}, size_x, size_y);
    risks[0][0] = 0;
    entered[0][0] = true;

    auto risk_at = [&](int x, int y) -> int
    {
        return cavern[y][x] - '0';
    };

    while (true)
    {
        const int &x = traveler.x;
        const int &y = traveler.y;

        for (const auto &offset : traveler.direction.offsets())
        {
            int ax = x + offset.x;
            int ay = y + offset.y;

            if (traveler.valid(offset) && !entered[ay][ax])
            {
                if (risks[ay][ax] > risks[y][x] + risk_at(ax, ay))
                {
                    risks[ay][ax] = risks[y][x] + risk_at(ax, ay);
                }

                auto traveler_copy = traveler;
                traveler_copy.visit(offset);

                if (queued.find(traveler_copy) == queued.end())
                {
                    entered_points.push({risks[ay][ax], traveler_copy});
                    queued[traveler_copy] = true;
                }
            }
        }

        if (traveler.is_at_end())
            DPRINT3(traveler.get_path('.'), '.', 1);

        if (!entered_points.empty())
        {
            traveler = entered_points.top().second;
            entered_points.pop();
            queued.erase(traveler);
            entered[y][x] = true;
            continue;
        }
        break;
    }

    // DPRINT(risks);
    return risks[size_y - 1][size_x - 1];
}
