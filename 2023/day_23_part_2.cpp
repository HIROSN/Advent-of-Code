// #define DTIMER_ON
// #include <debug_print.h>
#include <main.h>
#include <traveler.h>

#include <algorithm>
#include <map>
#include <stack>
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

    const aoc::Point start{1, 0};
    const aoc::Point end{size_x - 2, size_y - 1};

    std::map<aoc::Point, std::vector<std::pair<aoc::Point, int>>>
        trail_map;

    auto get_trail_info = [&]() -> void
    {
        std::vector<std::vector<int>> paths(
            size_y, std::vector<int>(size_x, 0));

        std::stack<std::pair<aoc::Point, aoc::Traveler>> stack;
        stack.emplace(std::make_pair(
            start, aoc::Traveler(start, end, size_x, size_y)));

        std::vector<std::string> junctions(
            size_y, std::string(size_x, '.'));

        auto get_offsets = [&](aoc::Traveler &traveler)
            -> std::vector<aoc::Offset>
        {
            std::vector<aoc::Offset> offsets;
            for (const auto &offset : traveler.direction.offsets())
            {
                if (traveler.valid(offset))
                {
                    const auto point = traveler.next_at(offset);
                    if (hiking_trails[point.y][point.x] != '#')
                        offsets.push_back(offset);
                }
            }
            return offsets;
        };

        while (!stack.empty())
        {
            auto trailhead = stack.top().first;
            auto traveler = stack.top().second;
            stack.pop();

            auto offsets = get_offsets(traveler);
            paths[traveler.y][traveler.x] = offsets.size();

            if (offsets.size() >= 3 || traveler.is_at_end())
            {
                const auto &p1 = trailhead;
                const auto &p2 = traveler.is_at();
                const int steps = traveler.visited.size() - 1;
                trail_map[p1].push_back(std::make_pair(p2, steps));
                trail_map[p2].push_back(std::make_pair(p1, steps));
                junctions[traveler.y][traveler.x] = 'X';
                trailhead = traveler.is_at();
                traveler.visited = {{traveler.direction.heading, trailhead}};
            }

            for (const auto &offset : offsets)
            {
                const auto point = traveler.next_at(offset);
                if (point == trailhead)
                    continue;

                if (!paths[point.y][point.x] ||
                    paths[point.y][point.x] >= 3)
                {
                    auto next = traveler;
                    next.visit(offset);
                    stack.push({trailhead, next});
                }
            }
        }

        trail_map[end].clear();
        DPRINT(junctions);
    };

    get_trail_info();

    for (auto it : trail_map)
    {
        DPRINTX(it.first << " -> ");
        for (const auto it : it.second)
            DPRINTX(it.first << "(" << it.second << ") ");
        DPRINT_ENDL();
    }

    struct Walk
    {
        aoc::Point at;
        std::vector<aoc::Point> visited;
        int total_steps = 0;
    };

    std::stack<Walk> stack;
    stack.emplace(Walk{start});
    int longest_hike = 0;

    while (!stack.empty())
    {
        auto walk = stack.top();
        stack.pop();

        if (walk.at == end)
        {
            if (walk.total_steps > longest_hike)
                longest_hike = walk.total_steps;

            DPRINTX(walk.total_steps << " ");
            for (auto point : walk.visited)
                DPRINTX(point);
            DPRINT_ENDL();
            continue;
        }

        for (auto it : trail_map[walk.at])
        {
            if (std::find(walk.visited.begin(), walk.visited.end(),
                          it.first) == walk.visited.end())
            {
                auto clone = walk;
                clone.at = it.first;
                clone.visited.push_back(it.first);
                clone.total_steps += it.second;
                stack.push(clone);
            }
        }
    }

    return longest_hike;
}
