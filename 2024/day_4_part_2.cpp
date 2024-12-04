#include <main.h>

#include <vector>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    std::string line;
    std::vector<std::string> word_search;

    while (std::getline(file, line))
    {
        word_search.push_back(line);
    }

    const int size_x = line.size();
    const int size_y = word_search.size();

    auto four_neighbors = [&](int x, int y) -> std::vector<char>
    {
        std::vector<char> neighbors;
        neighbors.push_back(word_search[y - 1][x - 1]);
        neighbors.push_back(word_search[y - 1][x + 1]);
        neighbors.push_back(word_search[y + 1][x + 1]);
        neighbors.push_back(word_search[y + 1][x - 1]);
        return neighbors;
    };

    int total = 0;

    for (int y = 1; y < size_y - 1; y++)
    {
        for (int x = 1; x < size_x - 1; x++)
        {
            if (word_search[y][x] != 'A')
                continue;

            auto neighbors = four_neighbors(x, y);
            bool xmas = neighbors[0] != neighbors[2] && neighbors[1] != neighbors[3];
            if (!xmas)
                continue;

            for (auto neighbor : neighbors)
            {
                if (neighbor != 'M' && neighbor != 'S')
                {
                    xmas = false;
                    break;
                }
            }

            if (!xmas)
                continue;

            total++;
        }
    }

    return total;
}
