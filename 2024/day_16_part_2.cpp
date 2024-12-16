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
    std::map<std::pair<Number, Number>, int> best_spot_to_sit;

    struct Reindeer : public aoc::Traveler
    {
        Number score = 0;

        Reindeer(const aoc::Point &ps, const aoc::Point &pe,
                 Number sx, Number sy) : aoc::Traveler(ps, pe, sx, sy)
        {
            direction.type = aoc::Direction::Type::TurnLeftFirst;
            direction.heading = aoc::Offset::Right;
        }

        void visit(const aoc::Offset &offset) override
        {
            score++;

            if (direction.heading != offset)
                score += 1000;

            aoc::Traveler::visit(offset);
        }

        std::vector<aoc::Offset> get_offsets(
            std::vector<std::string> &map)
        {
            std::vector<aoc::Offset> offsets;

            for (auto &offset : direction.offsets(3))
                if (is_visited(offset))
                    continue;
                else if (map[y + offset.y][x + offset.x] != '#')
                    offsets.push_back(offset);

            return offsets;
        };

        std::vector<std::string> debug_print(std::vector<std::string> map)
        {
            for (auto it : get_path('.'))
                if (map[it.first.second][it.first.first] == '.')
                    map[it.first.second][it.first.first] = it.second;
            return map;
        }
    };

    auto get_score = [&](Reindeer reindeer_start) -> Reindeer
    {
        std::vector<std::vector<Number>> lowest_score(
            size_y, std::vector<Number>(size_x, NUMBER_MAX));

        auto lowest_at = [&](const Reindeer &r) -> Number &
        {
            return lowest_score[r.y][r.x];
        };

        using ReindeerQueue =
            std::priority_queue<std::pair<Number, Reindeer>,
                                std::vector<std::pair<Number, Reindeer>>,
                                std::greater<std::pair<Number, Reindeer>>>;

        ReindeerQueue reindeer_queue;
        aoc::Point::priority = aoc::Point::Priority::Long;
        reindeer_queue.emplace(std::make_pair(0, reindeer_start));

        while (!reindeer_queue.empty())
        {
            auto reindeer = reindeer_queue.top().second;
            reindeer_queue.pop();

            if (reindeer.score >= lowest_at(reindeer))
                continue;

            lowest_at(reindeer) = reindeer.score;
            if (reindeer.is_at_end())
                return reindeer;

            for (const auto &offset : reindeer.get_offsets(map))
            {
                auto next = reindeer;
                next.visit(offset);
                reindeer_queue.push({next.score, next});
            }
        }

        return reindeer_start;
    };

    auto add_to_best_spot = [&](const Reindeer &r) -> void
    {
        for (auto it : r.visited)
            best_spot_to_sit[it.second.as_pair()]++;
    };

    Reindeer reindeer(start, end, size_x, size_y);
    Reindeer reindeer_lowest_score = get_score(reindeer);
    DPRINT(reindeer_lowest_score.debug_print(map));
    DPRINTX_ENDL(reindeer_lowest_score.score);
    add_to_best_spot(reindeer_lowest_score);

    for (auto it = reindeer_lowest_score.visited.begin();
         it != reindeer_lowest_score.visited.end();
         it++)
    {
        const auto &offset = it->first;
        const auto &point = it->second;

        if (point == start || point == end)
            continue;

        const auto &next_offset = (it + 1)->first;
        reindeer.visit(offset);

        for (const auto &next : reindeer.get_offsets(map))
        {
            if (next == next_offset)
                continue;

            auto reindeer_copy = reindeer;
            reindeer_copy.visit(next);
            Reindeer reindeer_at_end = get_score(reindeer_copy);

            if (reindeer_at_end.score != reindeer_lowest_score.score)
                continue;

            DPRINTX_ENDL(point << next.ch);
            add_to_best_spot(reindeer_at_end);
        }
    }

    auto debug_print = [&]() -> std::vector<std::string>
    {
        for (auto it : best_spot_to_sit)
            map[it.first.second][it.first.first] = 'O';
        return map;
    };

    DPRINT(debug_print());
    return best_spot_to_sit.size();
}
