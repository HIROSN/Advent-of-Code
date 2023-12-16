// #include <debug_print.h>
#include <main.h>

#include <algorithm>
#include <map>
#include <queue>
#include <utility>
#include <vector>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    static std::vector<std::string> contraption;
    std::string line;

    while (std::getline(file, line))
        contraption.push_back(line);

    DPRINT(contraption);
    static const int size_x = line.size();
    static const int size_y = contraption.size();

    using Heading = std::pair<int, int>;
    static const Heading right{1, 0};
    static const Heading down{0, 1};
    static const Heading left{-1, 0};
    static const Heading up{0, -1};

    using Key = std::pair<Heading, char>;
    static std::map<Key, std::vector<Heading>> behaviors{
        {{right, '/'}, {up}},
        {{right, '\\'}, {down}},
        {{right, '|'}, {up, down}},
        {{down, '/'}, {left}},
        {{down, '\\'}, {right}},
        {{down, '-'}, {left, right}},
        {{left, '/'}, {down}},
        {{left, '\\'}, {up}},
        {{left, '|'}, {up, down}},
        {{up, '/'}, {right}},
        {{up, '\\'}, {left}},
        {{up, '-'}, {left, right}},
    };

    struct Beam
    {
        int x;
        int y;
        Heading heading;

        std::vector<Beam> move() const
        {
            auto moved = *this;
            moved.x += heading.first;
            moved.y += heading.second;

            if (moved.x < 0 || moved.x >= size_x ||
                moved.y < 0 || moved.y >= size_y)
            {
                return {};
            }

            std::vector<Beam> new_beams;
            Key key{heading, encounters(moved)};

            if (behaviors.find(key) != behaviors.end())
            {
                for (const auto &new_heading : behaviors[key])
                    new_beams.push_back(reflected(moved, new_heading));
            }
            else
            {
                new_beams.push_back(moved);
            }

            return new_beams;
        }

        char encounters(const Beam &moved) const
        {
            return contraption[moved.y][moved.x];
        }

        Beam reflected(Beam moved, Heading new_heading) const
        {
            moved.heading = new_heading;
            return moved;
        }

        bool operator==(const Beam &right) const
        {
            return x == right.x && y == right.y && heading == right.heading;
        }
    };

    auto part_1 = [&](const Beam &edge) -> int
    {
        std::queue<Beam> beams;
        beams.push(edge);
        std::vector<Beam> visited;

        using Tile = std::pair<int, int>;
        std::map<Tile, char> energized;

        while (!beams.empty())
        {
            auto beam = beams.front();
            beams.pop();

            if (std::find(visited.begin(), visited.end(), beam) == visited.end())
                visited.push_back(beam);
            else
                continue;

            for (const auto &new_beam : beam.move())
            {
                beams.push(new_beam);

                Tile tile{new_beam.x, new_beam.y};
                if (energized.find(tile) == energized.end())
                    energized[tile] = '#';
            }
        }

        DPRINT3(energized, '.', 1);
        return energized.size();
    };

    std::priority_queue<int> number_of_tiles_energized;

    for (int x = 0; x < size_x; x++)
    {
        number_of_tiles_energized.push(part_1({x, -1, down}));
        number_of_tiles_energized.push(part_1({x, size_y, up}));
    }

    for (int y = 0; y < size_y; y++)
    {
        number_of_tiles_energized.push(part_1({-1, y, right}));
        number_of_tiles_energized.push(part_1({size_x, y, left}));
    }

    return number_of_tiles_energized.top();
}
