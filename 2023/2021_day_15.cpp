// #define DTIMER_ON
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

    DPRINT(cavern);

    const int size_x = line.size();
    const int size_y = cavern.size();

    std::vector<std::vector<int>> risk_map(
        size_y, std::vector<int>(size_x, INT_MAX));

    auto risk_at = [&](const aoc::Point &p) -> int
    {
        return cavern[p.y][p.x] - '0';
    };

    auto risks = [&](const aoc::Point &p) -> int &
    {
        return risk_map[p.y][p.x];
    };

    using TravelerQueue =
        std::priority_queue<std::pair<int, aoc::Traveler>,
                            std::vector<std::pair<int, aoc::Traveler>>,
                            std::greater<std::pair<int, aoc::Traveler>>>;

    TravelerQueue traveler_queue;
    std::map<aoc::Traveler, bool> queued;
    std::map<aoc::Point, bool> entered;

    const aoc::Point start{0, 0};
    const aoc::Point end{size_x - 1, size_y - 1};

    risks(start) = 0;
    entered[start] = true;
    aoc::Traveler traveler(start, end, size_x, size_y);

    while (true)
    {
        for (const auto &offset : traveler.direction.offsets(3))
        {
            if (traveler.valid(offset) && !entered[traveler.next_at(offset)])
            {
                auto next = traveler;
                next.visit(offset);

                if (risks(next.is_at()) >
                    risks(traveler.is_at()) + risk_at(next.is_at()))
                {
                    risks(next.is_at()) =
                        risks(traveler.is_at()) + risk_at(next.is_at());
                }

                if (queued.find(next) == queued.end())
                {
                    traveler_queue.push({risks(next.is_at()), next});
                    queued[next] = true;
                }
            }
        }

        if (traveler.is_at_end())
            DPRINT3(traveler.get_path('.'), '.', 1);

        if (!traveler_queue.empty())
        {
            traveler = traveler_queue.top().second;
            traveler_queue.pop();
            queued.erase(traveler);
            entered[traveler.is_at()] = true;
            continue;
        }
        break;
    }

    // DPRINT(risk_map);
    return risks(end);
}
