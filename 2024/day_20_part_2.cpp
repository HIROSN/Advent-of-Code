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
        {
            start = {sx, y};
            line[sx] = '.';
        }

        Number ex = line.find('E');
        if (ex != std::string::npos)
        {
            end = {ex, y};
            line[ex] = '.';
        }

        map.push_back(line);
    }

    DPRINT(map);
    const Number size_x = line.size();
    const Number size_y = map.size();

    using Racer = aoc::Traveler;
    std::vector<std::vector<Number>> fewest_picoseconds(
        size_y, std::vector<Number>(size_x, NUMBER_MAX));

    std::function<bool(Number, Number)> is_wall =
        [&](Number x, Number y) -> bool
    {
        if (x < 0 || x >= size_x || y < 0 || y >= size_y)
            return true;
        return map[y][x] == '#';
    };

    auto no_cheat_fewest = [&](Number x, Number y) -> Number
    {
        if (x < 0 || x >= size_x || y < 0 || y >= size_y)
            return 0;

        if (is_wall(x, y))
            return 0;

        return fewest_picoseconds[y][x] - 1;
    };

    auto race = [&]() -> Racer
    {
        auto fewest_at = [&](const Racer &r) -> Number &
        {
            return fewest_picoseconds[r.y][r.x];
        };

        using RacerQueue =
            std::priority_queue<std::pair<Number, Racer>,
                                std::vector<std::pair<Number, Racer>>,
                                std::greater<std::pair<Number, Racer>>>;

        RacerQueue racer_queue;
        racer_queue.emplace(
            std::make_pair(0, Racer(start, end, size_x, size_y)));

        while (!racer_queue.empty())
        {
            auto racer = racer_queue.top().second;
            racer_queue.pop();

            if (racer.visited.size() >= fewest_at(racer))
                continue;

            fewest_at(racer) = racer.visited.size();
            if (racer.is_at_end())
                return racer;

            for (const auto &offset : racer.direction.offsets(3))
            {
                if (is_wall(racer.x + offset.x, racer.y + offset.y))
                    continue;

                auto next = racer;
                next.visit(offset);
                racer_queue.push({next.visited.size(), next});
            }
        }

        return {};
    };

    using Cheat = std::pair<aoc::Point, aoc::Point>;
    std::map<int, std::vector<Cheat>> saving_cheats_map;
    int save_at_least_100_picoseconds = 0;
    const int rule = 20;

    for (auto it : race().get_path('.', 0, true))
    {
        Number x = it.first.first;
        Number y = it.first.second;
        std::vector<Cheat> all;

        for (auto o : aoc::FourNeighbors().offsets)
        {
            int sx = x + o.x;
            int sy = y + o.y;

            for (int ex = sx - (rule - 1); ex <= sx + (rule - 1); ex++)
            {
                for (int ey = sy - (rule - 1); ey <= sy + (rule - 1); ey++)
                {
                    int picoseconds = std::abs(x - ex) + std::abs(y - ey);

                    if (picoseconds > rule)
                        continue;

                    Cheat cheat{{x, y}, {ex, ey}};

                    if (is_wall(cheat.second.x, cheat.second.y))
                        continue;

                    auto use_cheat = no_cheat_fewest(x, y) + picoseconds;
                    auto no_cheat = no_cheat_fewest(cheat.second.x,
                                                    cheat.second.y);
                    int saving = no_cheat - use_cheat;

                    if (saving > 0)
                    {
                        if (std::find(all.begin(), all.end(), cheat) ==
                            all.end())
                        {
                            saving_cheats_map[saving].push_back(cheat);
                            all.push_back(cheat);
                        }
                    }
                }
            }
        }
    }

    for (auto it : saving_cheats_map)
    {
        const int &saving = it.first;
        const auto &cheats = it.second;

        if (saving >= 100)
            save_at_least_100_picoseconds += cheats.size();

        if (rule == 2 || saving >= 50)
            DPRINTX_ENDL("There are " << cheats.size() << " cheats that save "
                                      << saving << " picoseconds.");
    }

    return save_at_least_100_picoseconds;
}
