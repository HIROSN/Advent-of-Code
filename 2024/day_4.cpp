#include <main.h>

#include <regex>
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

    for (int x = 0; x < size_x; x++)
    {
        std::string vertical;

        for (int y = 0; y < size_y; y++)
            vertical += word_search[y][x];

        word_search.push_back(vertical);
    }

    for (int sy = -size_x; sy < size_y + size_x; sy++)
    {
        std::string diagonal_up, diagonal_down;

        for (int x = 0; x < size_x; x++)
        {
            int y = sy - x;
            if (y >= 0 && y < size_y)
                diagonal_up += word_search[y][x];

            y = sy + x;
            if (y >= 0 && y < size_y)
                diagonal_down += word_search[y][x];
        }

        if (diagonal_up.size() >= 4)
            word_search.push_back(diagonal_up);

        if (diagonal_down.size() >= 4)
            word_search.push_back(diagonal_down);
    }

    auto search = [](std::string line, const std::regex &word) -> int
    {
        int count = 0;
        std::smatch match;
        while (std::regex_search(line, match, word))
        {
            count++;
            line = match.suffix().str();
        }
        return count;
    };

    int total = 0;

    for (auto line : word_search)
    {
        total += search(line, std::regex("XMAS"));
        total += search(line, std::regex("SAMX"));
    }

    return total;
}
