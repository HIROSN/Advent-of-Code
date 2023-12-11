// #include <debug_print.h>
#include <main.h>

#include <cmath>
#include <utility>
#include <vector>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    const int npos = -1;
    std::vector<std::string> image;
    std::string line;

    while (std::getline(file, line))
        image.push_back(line);

    DPRINT(image);

    auto size_x = [&]() -> int
    {
        return image[0].size();
    };

    auto size_y = [&]() -> int
    {
        return image.size();
    };

    auto find = [&](int x, char ch) -> int
    {
        for (int y = 0; y < size_y(); y++)
            if (image[y][x] == ch)
                return y;
        return npos;
    };

    for (int x = size_x() - 1; x >= 0; x--)
    {
        if (find(x, '#') == npos)
            for (int y = 0; y < size_y(); y++)
                image[y].insert(x, ".");
    }

    for (int y = size_y() - 1; y >= 0; y--)
    {
        if (image[y].find('#') == std::string::npos)
            image.insert(image.begin() + y, std::string(size_x(), '.'));
    }

    DPRINT(image);
    using Point = std::pair<int, int>;
    std::vector<Point> galaxies;
    int sum_of_lengths = 0;

    for (int y = 0; y < size_y(); y++)
        for (int x = 0; x < size_x(); x++)
            if (image[y][x] == '#')
                galaxies.push_back({x, y});

    auto length = [](const Point &a, const Point &b) -> int
    {
        return std::abs(a.first - b.first) + std::abs(a.second - b.second);
    };

    for (int a = 0; a < galaxies.size() - 1; a++)
        for (int b = a + 1; b < galaxies.size(); b++)
            sum_of_lengths += length(galaxies[a], galaxies[b]);

    return sum_of_lengths;
}
