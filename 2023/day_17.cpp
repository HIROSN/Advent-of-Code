// #define DTIMER_ON
// #include <debug_print.h>
#include <main.h>
#include <traveler.h>

#include <climits>
#include <functional>
#include <queue>
#include <utility>
#include <vector>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    struct Crucible : public aoc::Traveler
    {
        int heat_loss = 0;

        Crucible(const aoc::Point &ps, const aoc::Point &pe,
                 Number sx, Number sy) : aoc::Traveler(ps, pe, sx, sy) {}
    };

    std::vector<std::string> heat_map;
    std::string line;

    while (file >> line)
        heat_map.push_back(line);

    DPRINT(heat_map);

    const int size_x = line.size();
    const int size_y = heat_map.size();
    const int straight_at_most = 3;

    std::vector<std::vector<std::vector<std::vector<int>>>> least_map(
        size_y, std::vector<std::vector<std::vector<int>>>(
                    size_x, std::vector<std::vector<int>>(
                                aoc::Neighbors::number_of_offsets,
                                std::vector<int>(straight_at_most + 1,
                                                 INT_MAX))));

    auto straight_line = [&](const Crucible &c) -> int
    {
        int i;
        const auto last = c.visited.rbegin();
        for (i = 0; last + i != c.visited.rend(); i++)
        {
            if (last->first.ch == (last + i)->first.ch)
                continue;
            break;
        }
        return i;
    };

    auto heat_at = [&](const aoc::Point &p) -> int
    {
        return heat_map[p.y][p.x] - '0';
    };

    auto least_at = [&](const Crucible &c) -> int &
    {
        DCHECK(straight_line(c) <= straight_at_most);
        return least_map[c.y][c.x]
                        [c.direction.neighbor_index()]
                        [straight_line(c)];
    };

    using CrucibleQueue =
        std::priority_queue<std::pair<int, Crucible>,
                            std::vector<std::pair<int, Crucible>>,
                            std::greater<std::pair<int, Crucible>>>;

    const aoc::Point start{0, 0};
    const aoc::Point end{size_x - 1, size_y - 1};

    CrucibleQueue crucible_queue;
    crucible_queue.emplace(
        std::make_pair(0, Crucible(start, end, size_x, size_y)));

    std::priority_queue<int, std::vector<int>, std::greater<int>>
        least_heat_loss;

    while (!crucible_queue.empty())
    {
        auto crucible = crucible_queue.top().second;
        crucible_queue.pop();

        if (crucible.heat_loss >= least_at(crucible))
            continue;

        least_at(crucible) = crucible.heat_loss;
        if (crucible.is_at_end())
        {
            least_heat_loss.push(crucible.heat_loss);
            DPRINT3(crucible.get_path('.'), '.', 1);
            DPRINTX_ENDL(crucible.heat_loss);
            continue;
        }

        for (const auto &offset : crucible.direction.offsets(3))
        {
            if (!crucible.valid(offset) || crucible.is_visited(offset))
                continue;

            if (offset != crucible.direction.heading ||
                straight_line(crucible) < straight_at_most)
            {
                auto next = crucible;
                next.visit(offset);
                next.heat_loss += heat_at(next.is_at());
                crucible_queue.push({next.heat_loss, next});
            }
        }
    }

    return least_heat_loss.top();
}
