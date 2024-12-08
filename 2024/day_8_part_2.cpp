#include <main.h>

#include <map>
#include <utility>
#include <vector>

#define X 0
#define Y 1

std::optional<uint64_t> Answer(std::ifstream &file)
{
    using Point = std::pair<int, int>;
    std::map<char, std::vector<Point>> antennas;
    std::string line;
    int y = 0;

    while (std::getline(file, line))
    {
        for (int x = 0; x < line.size(); x++)
            if (line[x] != '.')
                antennas[line[x]].push_back({x, y});
        y++;
    }

    const int size_x = line.size();
    const int size_y = y;
    std::map<Point, int> antinodes;

    auto is_antinode = [&](const Point &p) -> bool
    {
        const int x = std::get<X>(p);
        const int y = std::get<Y>(p);
        return x >= 0 && x < size_x && y >= 0 && y < size_y;
    };

    for (auto it : antennas)
    {
        for (int i = 0; i < it.second.size() - 1; i++)
        {
            for (int j = i + 1; j < it.second.size(); j++)
            {
                const auto &p1 = it.second[i];
                const auto &p2 = it.second[j];
                const int x1 = std::get<X>(p1);
                const int y1 = std::get<Y>(p1);
                const int x2 = std::get<X>(p2);
                const int y2 = std::get<Y>(p2);
                const int dx1 = x1 - x2;
                const int dy1 = y1 - y2;
                const int dx2 = x2 - x1;
                const int dy2 = y2 - y1;

                for (int n = 0; is_antinode({x1 + dx1 * n, y1 + dy1 * n}); n++)
                    antinodes[{x1 + dx1 * n, y1 + dy1 * n}]++;

                for (int n = 0; is_antinode({x2 + dx2 * n, y2 + dy2 * n}); n++)
                    antinodes[{x2 + dx2 * n, y2 + dy2 * n}]++;
            }
        }
    }

    return antinodes.size();
}
