#include <main.h>

#include <algorithm>
#include <cctype>
#include <map>
#include <sstream>
#include <unordered_set>
#include <utility>
#include <vector>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    std::vector<std::string> schematic;
    std::string line;

    while (std::getline(file, line))
        schematic.push_back(line);

    int size_x = line.size();
    int size_y = schematic.size();

    using Point = std::pair<int, int>;
    using Points = std::vector<Point>;

    auto find_adjacent_stars = [&](int x, int y, Points &stars) -> void
    {
        for (int ay = y - 1; ay <= y + 1; ay++)
        {
            if (ay < 0 || ay >= size_y)
                continue;

            for (int ax = x - 1; ax <= x + 1; ax++)
            {
                if (ax < 0 || ax >= size_x)
                    continue;

                if (schematic[ay][ax] == '*')
                {
                    Point star = {ax, ay};
                    if (stars.empty() || std::find(stars.begin(),
                                                   stars.end(),
                                                   star) == stars.end())
                    {
                        stars.push_back(std::move(star));
                    }
                }
            }
        }
    };

    auto last_digit = [&](int x, int y) -> bool
    {
        return ++x >= size_x || !std::isdigit(schematic[y][x]);
    };

    struct Part
    {
        std::string digits;
        Points stars;

        int part_number()
        {
            int number;
            std::stringstream(digits) >> number;
            return number;
        }
    };

    Part part;
    std::vector<int> part_numbers;
    std::map<Point, std::unordered_set<int>> parts_adjacent_to_star;

    for (int y = 0; y < size_y; y++)
    {
        for (int x = 0; x < size_x; x++)
        {
            const char &ch = schematic[y][x];
            if (std::isdigit(ch))
            {
                part.digits += ch;
                find_adjacent_stars(x, y, part.stars);

                if (last_digit(x, y))
                {
                    const int part_id = part_numbers.size();
                    for (auto star : part.stars)
                        parts_adjacent_to_star[star].insert(part_id);
                    part_numbers.push_back(part.part_number());
                    part = {};
                }
            }
        }
    }

    int sum_of_gear_ratios = 0;
    for (auto it : parts_adjacent_to_star)
    {
        if (it.second.size() == 2)
        {
            int gear_ratio = 1;
            for (auto part_id : it.second)
                gear_ratio *= part_numbers[part_id];
            sum_of_gear_ratios += gear_ratio;
        }
    }

    return sum_of_gear_ratios;
}
