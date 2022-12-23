#include <main.h>

#include <algorithm>
#include <functional>
#include <list>
#include <map>
#include <utility>
#include <vector>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    std::vector<std::string> grove;
    std::string line;
    while (std::getline(file, line))
        grove.push_back(line);

    const int half_x = line.size() / 2;
    const int half_y = grove.size() / 2;
    const char mark_elf = '#';
    const char mark_ground = '.';

    using Position = std::pair<int64_t, int64_t>;
    using Direction = Position;

    std::map<Position, char> elves;
    for (int x = 0; x < line.size(); x++)
        for (int y = 0; y < grove.size(); y++)
            if (grove[y][x] == mark_elf)
                elves[{x - half_x, half_y - y}] = mark_elf;

    std::map<std::string, Direction> directions = {
        {"N", {0, 1}},
        {"S", {0, -1}},
        {"W", {-1, 0}},
        {"E", {1, 0}},
        {"NE", {1, 1}},
        {"NW", {-1, 1}},
        {"SE", {1, -1}},
        {"SW", {-1, -1}},
    };

    auto adjacent_position =
        [](const Position &p,
           const Direction &d) -> Position
    {
        return {p.first + d.first, p.second + d.second};
    };

    auto no_elf = [&](const Position &p, std::string direction) -> bool
    {
        const auto adjacent = adjacent_position(p, directions[direction]);
        return elves.find(adjacent) == elves.end();
    };

    std::list<std::vector<std::string>> direction_list = {
        {"N", "NE", "NW"},
        {"S", "SE", "SW"},
        {"W", "NW", "SW"},
        {"E", "NE", "SE"}};

    int64_t round = 0;

    while (++round)
    {
        std::map<Position, Position> proposed;
        std::map<Position, int> number_of_proposes;
        std::map<Position, char> elves_new;
        int elves_moved = 0;

        for (const auto &elf : elves)
        {
            const auto &current = elf.first;
            bool no_other_elves = true;

            for (const auto &direction : directions)
            {
                if (!no_elf(current, direction.first))
                {
                    no_other_elves = false;
                    break;
                }
            }

            if (no_other_elves)
                continue;

            for (const auto &consider : direction_list)
            {
                bool no_elf_in_direction = true;
                for (const auto &direction : consider)
                {
                    if (!no_elf(current, direction))
                    {
                        no_elf_in_direction = false;
                        break;
                    }
                }

                if (no_elf_in_direction)
                {
                    auto adjacent = adjacent_position(
                        current,
                        directions[consider[0]]);

                    proposed[current] = adjacent;
                    number_of_proposes[adjacent]++;
                    break;
                }
            }
        }

        for (auto &elf : elves)
        {
            auto &current = elf.first;

            if (proposed.find(current) != proposed.end())
            {
                const auto &adjacent = proposed[current];

                if (number_of_proposes[adjacent] == 1)
                {
                    elves_new[adjacent] = mark_elf;
                    elves_moved++;
                    continue;
                }
            }

            elves_new[current] = mark_elf;
        }

        if (!elves_moved)
            break;

        elves = std::move(elves_new);

        direction_list.push_back(direction_list.front());
        direction_list.pop_front();
    }

    return round;
}
