// #include <debug_print.h>
#include <main.h>

#include <map>
#include <utility>
#include <vector>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    using Point = std::pair<int, int>;
    using Direction = Point;
    const Direction up = {0, -1}, right = {1, 0}, down = {0, 1}, left = {-1, 0};
    const std::vector<Direction> neighbors{up, right, down, left};

    std::map<Direction, std::map<char, Direction>> connected{
        {up, {{'|', up}, {'7', left}, {'F', right}}},
        {right, {{'-', right}, {'J', up}, {'7', down}}},
        {down, {{'|', down}, {'L', right}, {'J', left}}},
        {left, {{'-', left}, {'L', up}, {'F', down}}}};

    std::vector<std::string> sketch;
    Point start;
    std::string line;

    for (int y = 1; std::getline(file, line); y++)
    {
        line = ' ' + line + ' ';
        if (y == 1)
            sketch.push_back(std::string(line.size(), ' '));
        sketch.push_back(line);
        auto x = line.find('S');
        if (x != std::string::npos)
            start = {x, y};
    }
    sketch.push_back(std::string(line.size(), ' '));

    DPRINT(sketch);
    DPRINT(start);
    DPRINT_ENDL();

    auto get_tile = [&](const Point &position,
                        const Direction &next) -> char
    {
        int x = position.first + next.first;
        int y = position.second + next.second;
        return sketch[y][x];
    };

    auto is_connected = [&](const Point &position,
                            const Direction &next) -> bool
    {
        const auto &map = connected[next];
        return map.find(get_tile(position, next)) != map.end();
    };

    Point position1 = start, position2 = start;
    Direction next1, next2;
    int steps = 0, i;

    for (i = 0; i < neighbors.size(); i++)
    {
        next1 = neighbors[i];
        if (is_connected(position1, next1))
            break;
    }

    for (i++; i < neighbors.size(); i++)
    {
        next2 = neighbors[i];
        if (is_connected(position2, next2))
            break;
    }

    auto do_step = [&](Point &position,
                       Direction &next) -> void
    {
        auto tile = get_tile(position, next);
        position.first += next.first;
        position.second += next.second;
        next = connected[next][tile];
    };

    while (++steps)
    {
        do_step(position1, next1);
        do_step(position2, next2);

        DPRINT(position1);
        DPRINT(position2);
        DPRINT_ENDL();

        if (position1 == position2)
            break;
    }

    return steps;
}
