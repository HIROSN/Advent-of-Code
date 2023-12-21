// #include <debug_print.h>
#include <main.h>

#include <utility>
#include <vector>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    std::vector<std::string> garden;
    std::string line;

    while (std::getline(file, line))
        garden.push_back(line);

    const int size_x = line.size();
    const int size_y = garden.size();
    DPRINT(garden);

    std::vector<std::vector<int>> steps(size_y,
                                        std::vector<int>(size_x, 0));
    using Point = std::pair<int, int>;

    auto garden_at = [&](const Point &p) -> char &
    {
        return garden[p.second][p.first];
    };

    auto steps_at = [&](const Point &p) -> int &
    {
        return steps[p.second][p.first];
    };

    auto four_neighbors = [&](int x, int y) -> std::vector<Point>
    {
        static const std::vector<Point>
            the_four{{1, 0}, {0, 1}, {-1, 0}, {0, -1}};
        std::vector<Point> vp;
        for (const auto &p : the_four)
        {
            const int ax = x + p.first;
            const int ay = y + p.second;
            if (ax >= 0 && ax < size_x &&
                ay >= 0 && ay < size_y)
            {
                vp.push_back({ax, ay});
            }
        }
        return vp;
    };

    auto do_step = [&](char find, int step) -> int
    {
        int plots = 0;
        for (int y = 0; y < size_y; y++)
        {
            for (int x = 0; x < size_x; x++)
            {
                if (garden_at({x, y}) == find &&
                    steps_at({x, y}) <= step)
                {
                    for (const auto &ap : four_neighbors(x, y))
                    {
                        if (garden_at(ap) == '.')
                        {
                            garden_at(ap) = 'O';
                            steps_at(ap) = step + 1;
                            plots++;
                        }
                        garden_at({x, y}) = '.';
                    }
                }
            }
        }
        // DPRINT(garden);
        // DPRINT(steps);
        return plots;
    };

    int garden_plots;

    for (int step = 0; step < 64; step++)
    {
        garden_plots = do_step(!step ? 'S' : 'O', step);
    }

    DPRINT(garden);
    return garden_plots;
}
