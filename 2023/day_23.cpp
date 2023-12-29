// #define DTIMER_ON
// #include <debug_print.h>
#include <main.h>
#include <traveler.h>

#include <functional>
#include <map>
#include <queue>
#include <utility>
#include <vector>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    std::vector<std::string> hiking_trails;
    std::string line;

    while (file >> line)
        hiking_trails.push_back(line);

    DPRINT(hiking_trails);

    const int size_x = line.size();
    const int size_y = hiking_trails.size();

    std::vector<std::vector<int>> hiking_map(
        size_y, std::vector<int>(size_x, 0));

    auto tile_at = [&](const aoc::Point &p) -> char
    {
        return hiking_trails[p.y][p.x];
    };

    auto longest_hike = [&](const aoc::Point &p) -> int &
    {
        return hiking_map[p.y][p.x];
    };

    using TravelerQueue =
        std::priority_queue<std::pair<int, aoc::Traveler>,
                            std::vector<std::pair<int, aoc::Traveler>>,
                            std::less<std::pair<int, aoc::Traveler>>>;

    TravelerQueue travelers;
    std::map<aoc::Traveler, bool> queued;

    const aoc::Point start{1, 0};
    const aoc::Point end{size_x - 2, size_y - 1};
    aoc::Traveler traveler(start, end, size_x, size_y);

    auto get_offsets = [&](const aoc::Point &p) -> std::vector<aoc::Offset>
    {
        const std::map<char, std::vector<aoc::Offset>> slopes{
            {'^', {aoc::Offset::Up}},
            {'>', {aoc::Offset::Right}},
            {'v', {aoc::Offset::Down}},
            {'<', {aoc::Offset::Left}}};

        auto it = slopes.find(tile_at(p));
        if (it != slopes.end())
            return it->second;

        return traveler.direction.offsets(3);
    };

    while (true)
    {
        for (const auto &offset : get_offsets(traveler.is_at()))
        {
            if (traveler.valid(offset) &&
                tile_at(traveler.next_at(offset)) != '#' &&
                !traveler.is_visited(offset) &&
                longest_hike(traveler.next_at(offset)) < traveler.visited.size())
            {
                auto next = traveler;
                next.visit(offset);
                longest_hike(next.is_at()) = traveler.visited.size();

                if (queued.find(next) == queued.end())
                {
                    travelers.push({next.visited.size(), next});
                    queued[next] = true;
                }
            }
        }

        if (traveler.is_at_end())
            DPRINT3(traveler.get_path('.', 'O'), '.', 1);

        if (!travelers.empty())
        {
            traveler = travelers.top().second;
            travelers.pop();
            queued.erase(traveler);
            continue;
        }
        break;
    }

    DPRINT(hiking_map);
    return longest_hike(end);
}
