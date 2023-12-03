#include <main.h>

#include <cctype>
#include <sstream>
#include <vector>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    std::vector<std::string> schematic;
    std::string line;

    while (std::getline(file, line))
        schematic.push_back(line);

    int size_x = line.size();
    int size_y = schematic.size();

    auto adjacent_to_symbol = [&](int x, int y) -> bool
    {
        bool adjacent = false;
        for (int ay = y - 1; !adjacent && ay <= y + 1; ay++)
        {
            if (ay < 0 || ay >= size_y)
                continue;

            for (int ax = x - 1; !adjacent && ax <= x + 1; ax++)
            {
                if (ax < 0 || ax >= size_x)
                    continue;

                const char &ch = schematic[ay][ax];
                adjacent = ch != '.' && !std::isdigit(ch);
            }
        }
        return adjacent;
    };

    auto last_digit = [&](int x, int y) -> bool
    {
        return ++x >= size_x || !std::isdigit(schematic[y][x]);
    };

    struct Part
    {
        std::string digits;
        bool is_adjacent_to_symbol = false;

        int part_number()
        {
            int number;
            std::stringstream(digits) >> number;
            return number;
        }
    };

    Part part;
    std::vector<Part> parts;

    for (int y = 0; y < size_y; y++)
    {
        for (int x = 0; x < size_x; x++)
        {
            const char &ch = schematic[y][x];
            if (std::isdigit(ch))
            {
                part.digits += ch;
                part.is_adjacent_to_symbol |= adjacent_to_symbol(x, y);

                if (last_digit(x, y))
                {
                    parts.push_back(std::move(part));
                    part = {};
                }
            }
        }
    }

    int sum_of_part_numbers = 0;
    for (auto part : parts)
    {
        if (part.is_adjacent_to_symbol)
            sum_of_part_numbers += part.part_number();
    }

    return sum_of_part_numbers;
}
