// #include <debug_print.h>
#include <main.h>

#include <algorithm>
#include <map>
#include <utility>
#include <vector>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    using Point = std::pair<int, int>;
    using Direction = Point;
    using Directions = std::vector<Direction>;
    const Direction up = {0, -1}, right = {1, 0},
                    down = {0, 1}, left = {-1, 0};
    const std::vector<Direction> neighbors{up, right, down, left};

    std::map<Direction, std::map<char, Direction>> connected{
        {up, {{'|', up}, {'7', left}, {'F', right}}},
        {right, {{'-', right}, {'J', up}, {'7', down}}},
        {down, {{'|', down}, {'L', right}, {'J', left}}},
        {left, {{'-', left}, {'L', up}, {'F', down}}}};

    std::vector<std::string> sketch;
    Point start_position;
    Direction start_next;
    std::string line;

    for (int y = 1; std::getline(file, line); y++)
    {
        line = ' ' + line + ' ';
        if (y == 1)
            sketch.push_back(std::string(line.size(), ' '));
        sketch.push_back(line);
        auto x = line.find('S');
        if (x != std::string::npos)
            start_position = {x, y};
    }
    sketch.push_back(std::string(line.size(), ' '));

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

    for (const auto &neighbor : neighbors)
    {
        if (is_connected(start_position, neighbor))
        {
            start_next = neighbor;
            break;
        }
    }

    DPRINT(sketch);
    DPRINT(start_position);
    DPRINT(start_next);
    DPRINT_ENDL();

    std::vector<std::string> loop;
    std::vector<std::string> in_out;

    for (int y = 0; y < sketch.size(); y++)
    {
        line = std::string(sketch[0].size(), ' ');
        loop.push_back(line);
        in_out.push_back(line);
    }

    const char zero = '0';
    const char one = '1';

    const Direction up_left = {-1, -1}, up_right = {+1, -1},
                    down_left = {-1, +1}, down_right = {+1, +1};

    std::map<char, std::pair<Directions, Directions>> adjacents{
        {'F', {{up_left, up, up_right, down_left, left}, {down_right}}},
        {'7', {{up_left, up, up_right, right, down_right}, {down_left}}},
        {'J', {{up_right, right, down_right, down, down_left}, {up_left}}},
        {'L', {{up_left, down_right, down, down_left, left}, {up_right}}},
        {'-', {{up_left, up, up_right}, {down_right, down, down_left}}},
        {'|', {{up_left, down_left, left}, {up_right, right, down_right}}}};

    std::map<std::pair<char, Direction>, std::pair<char, char>> in_or_out_map{
        {{'F', right}, {zero, one}},
        {{'7', down}, {zero, one}},
        {{'J', left}, {zero, one}},
        {{'L', up}, {zero, one}},
        {{'F', down}, {one, zero}},
        {{'L', right}, {one, zero}},
        {{'J', up}, {one, zero}},
        {{'7', left}, {one, zero}},
        {{'|', up}, {zero, one}},
        {{'-', right}, {zero, one}},
        {{'|', down}, {one, zero}},
        {{'-', left}, {one, zero}}};

    auto set_in_out = [&](int x, int y, Direction adjacent, char ch) -> void
    {
        int ax = x + adjacent.first;
        int ay = y + adjacent.second;

        if (loop[ay][ax] == ' ')
            loop[ay][ax] = ch;
    };

    auto do_step = [&](Point &position,
                       Direction &next) -> void
    {
        auto tile = get_tile(position, next);
        position.first += next.first;
        position.second += next.second;
        next = connected[next][tile];
    };

    Point position = start_position;
    Direction next = start_next;

    while (true)
    {
        int x = position.first;
        int y = position.second;
        char tile = sketch[y][x];
        loop[y][x] = tile;

        if (in_or_out_map.find({tile, next}) != in_or_out_map.end())
        {
            auto in_or_out = in_or_out_map[{tile, next}];

            for (auto adjacent : std::get<0>(adjacents[tile]))
                set_in_out(x, y, adjacent, std::get<0>(in_or_out));

            for (auto adjacent : std::get<1>(adjacents[tile]))
                set_in_out(x, y, adjacent, std::get<1>(in_or_out));
        }

        do_step(position, next);

        if (position == start_position)
            break;
    }

    DPRINT(loop);
    const char in = 'I';
    const char out = 'O';
    int tiles_enclosed = 0;

    std::map<char, char> char_map{
        {'F', '.'},
        {'7', '.'},
        {'J', '.'},
        {'L', '.'},
        {'-', '.'},
        {'|', '.'}};

    for (int y = 0; y < loop.size(); y++)
    {
        for (int x = 0; x < loop[0].size(); x++)
        {
            char tile = loop[y][x];

            if (char_map.find(zero) == char_map.end())
            {
                if (tile == zero)
                {
                    char_map[zero] = out;
                    char_map[one] = in;
                }
                else if (tile == one)
                {
                    char_map[one] = out;
                    char_map[zero] = in;
                }
            }

            if (char_map.find(tile) != char_map.end())
            {
                tile = char_map[tile];
                in_out[y][x] = tile;
            }

            if (x == 0 || y == 0)
                continue;

            switch (tile)
            {
            case in:
                tiles_enclosed++;
                break;
            case ' ':
                if (in_out[y - 1][x] == in || in_out[y][x - 1] == in)
                {
                    in_out[y][x] = in;
                    tiles_enclosed++;
                }
                break;
            }
        }
    }

    DPRINT(in_out);
    return tiles_enclosed;
}
