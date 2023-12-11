#include <main.h>

#include <cmath>
#include <memory>
#include <utility>
#include <vector>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    using Point = std::pair<int64_t, int64_t>;
    const int64_t expand = 1000000 - 1;
    std::unique_ptr<Point> image[140][140];
    std::string line;
    int64_t size_x, size_y;

    for (int64_t y = 0; std::getline(file, line); y++)
    {
        if (y == 0)
            size_x = line.size();
        size_y++;

        for (int64_t x = 0; x < size_x; x++)
        {
            if (line[x] == '#')
            {
                image[x][y] = std::make_unique<Point>(x, y);
            }
        }
    }

    for (int64_t rx = size_x - 1; rx >= 0; rx--)
    {
        bool galaxies = false;
        for (int64_t y = 0; !galaxies && y < size_y; y++)
            galaxies |= image[rx][y] != nullptr;

        if (galaxies)
            continue;

        for (int64_t ex = rx + 1; ex < size_x; ex++)
            for (int64_t y = 0; y < size_y; y++)
                if (image[ex][y])
                    image[ex][y]->first += expand;
    }

    for (int64_t ry = size_y - 1; ry >= 0; ry--)
    {
        bool galaxies = false;
        for (int64_t x = 0; !galaxies && x < size_x; x++)
            galaxies |= image[x][ry] != nullptr;

        if (galaxies)
            continue;

        for (int64_t ey = ry + 1; ey < size_y; ey++)
            for (int64_t x = 0; x < size_x; x++)
                if (image[x][ey])
                    image[x][ey]->second += expand;
    }

    std::vector<Point> galaxies;
    int64_t sum_of_lengths = 0;

    for (int64_t x = 0; x < size_x; x++)
        for (int64_t y = 0; y < size_y; y++)
            if (image[x][y])
                galaxies.emplace_back(std::move(*image[x][y].release()));

    auto length = [](const Point &a, const Point &b) -> int64_t
    {
        return std::abs(a.first - b.first) + std::abs(a.second - b.second);
    };

    for (int64_t a = 0; a < galaxies.size() - 1; a++)
        for (int64_t b = a + 1; b < galaxies.size(); b++)
            sum_of_lengths += length(galaxies[a], galaxies[b]);

    return sum_of_lengths;
}
