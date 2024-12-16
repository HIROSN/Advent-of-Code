// #include <debug_print.h>
#include <main.h>
#include <traveler.h>

#include <functional>
#include <queue>
#include <utility>
#include <vector>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    std::vector<std::string> map;
    aoc::Point start, end;
    std::string line;

    while (file >> line)
    {
        Number y = map.size();

        Number sx = line.find('S');
        if (sx != std::string::npos)
            start = {sx, y};

        Number ex = line.find('E');
        if (ex != std::string::npos)
            end = {ex, y};

        map.push_back(line);
    }

    const Number size_x = line.size();
    const Number size_y = map.size();

    std::vector<std::vector<Number>> lowest_score(
        size_y, std::vector<Number>(size_x, NUMBER_MAX));

    struct Reindeer : public aoc::Traveler
    {
        Number score = 0;

        Reindeer(const aoc::Point &ps, const aoc::Point &pe,
                 Number sx, Number sy) : aoc::Traveler(ps, pe, sx, sy)
        {
            direction.type = aoc::Direction::Type::TurnLeftFirst;
            direction.heading = aoc::Offset::Right;
        }

        std::vector<std::string> debug_print(std::vector<std::string> map)
        {
            for (auto it : get_path('.'))
                if (map[it.first.second][it.first.first] == '.')
                    map[it.first.second][it.first.first] = it.second;
            return map;
        }
    };

    auto lowest_at = [&](const Reindeer &r) -> Number &
    {
        return lowest_score[r.y][r.x];
    };

    auto is_wall = [&](const Reindeer &r, const aoc::Offset &o) -> bool
    {
        return map[r.y + o.y][r.x + o.x] == '#';
    };

    using ReindeerQueue =
        std::priority_queue<std::pair<Number, Reindeer>,
                            std::vector<std::pair<Number, Reindeer>>,
                            std::greater<std::pair<Number, Reindeer>>>;

    ReindeerQueue reindeer_queue;
    aoc::Point::priority = aoc::Point::Priority::Long;
    reindeer_queue.emplace(
        std::make_pair(0, Reindeer(start, end, size_x, size_y)));

    while (!reindeer_queue.empty())
    {
        auto reindeer = reindeer_queue.top().second;
        reindeer_queue.pop();

        if (reindeer.score >= lowest_at(reindeer))
            continue;

        lowest_at(reindeer) = reindeer.score;
        if (reindeer.is_at_end())
        {
            DPRINT(reindeer.debug_print(map));
            continue;
        }

        for (const auto &offset : reindeer.direction.offsets(3))
        {
            if (is_wall(reindeer, offset))
                continue;

            auto next = reindeer;
            if (offset != reindeer.direction.heading)
                next.score += 1000;
            next.visit(offset);
            next.score++;
            reindeer_queue.push({next.score, next});
        }
    }

    return lowest_score[end.y][end.x];
}
