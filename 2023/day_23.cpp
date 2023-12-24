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

    int size_x = line.size();
    int size_y = hiking_trails.size();
    DPRINT(hiking_trails);

    std::vector<std::vector<int>> longest_hike;

    for (int y = 0; y < size_y; y++)
        longest_hike.emplace_back(size_x, 0);

    using TravelerQueue =
        std::priority_queue<std::pair<int, aoc::Traveler>,
                            std::vector<std::pair<int, aoc::Traveler>>,
                            std::less<std::pair<int, aoc::Traveler>>>;

    TravelerQueue travelers;
    std::map<aoc::Traveler, bool> queued;
    aoc::Point::priority = aoc::Point::Priority::Near;
    aoc::Traveler traveler({1, 0}, {size_x - 2, size_y - 1}, size_x, size_y);
    traveler.direction.type = aoc::Direction::Type::TurnFirst;
    int steps_of_longest_hike = 0;

    auto get_offsets = [&](int x, int y) -> std::vector<aoc::Offset>
    {
        std::vector<aoc::Offset> offsets;
        switch (hiking_trails[y][x])
        {
        case '^':
            offsets = {aoc::Offset::Up};
            break;
        case '>':
            offsets = {aoc::Offset::Right};
            break;
        case 'v':
            offsets = {aoc::Offset::Down};
            break;
        case '<':
            offsets = {aoc::Offset::Left};
            break;
        default:
            offsets = traveler.direction.offsets();
            break;
        }
        return offsets;
    };

    while (true)
    {
        const int &x = traveler.x;
        const int &y = traveler.y;

        for (const auto &offset : get_offsets(x, y))
        {
            int ax = x + offset.x;
            int ay = y + offset.y;

            if (traveler.valid(offset) && hiking_trails[ay][ax] != '#' &&
                !traveler.is_visited(offset))
            {
                if (longest_hike[ay][ax] < longest_hike[y][x] + 1)
                {
                    longest_hike[ay][ax] = longest_hike[y][x] + 1;
                }

                auto traveler_copy = traveler;
                traveler_copy.visit(offset);

                if (queued.find(traveler_copy) == queued.end())
                {
                    travelers.push({longest_hike[ay][ax], traveler_copy});
                    queued[traveler_copy] = true;
                }
            }
        }

        if (traveler.is_at_end())
        {
            int steps = traveler.visited.size() - 1;
            if (steps > steps_of_longest_hike)
                steps_of_longest_hike = steps;

            DPRINT3(traveler.get_path('.', 'O'), '.', 1);
            DPRINTX_ENDL(steps);
        }

        if (!travelers.empty())
        {
            traveler = travelers.top().second;
            travelers.pop();
            queued.erase(traveler);
            continue;
        }
        break;
    }

    DPRINT(longest_hike);
    return steps_of_longest_hike;
}
