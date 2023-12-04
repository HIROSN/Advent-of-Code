#include <main.h>

#include <algorithm>
#include <sstream>
#include <vector>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    std::string line;
    unsigned int total_points = 0;

    while (std::getline(file, line))
    {
        const auto colon = line.find(':');
        const auto bar = line.find('|');
        std::stringstream ss_left(line.substr(colon + 1, bar - colon - 1));
        std::stringstream ss_right(line.substr(bar + 1));
        std::vector<int> winning_numbers;
        std::vector<int> numbers;
        int number, points = 0;

        while (ss_left >> number)
            winning_numbers.push_back(number);
        while (ss_right >> number)
            numbers.push_back(number);

        std::sort(winning_numbers.begin(), winning_numbers.end());
        std::sort(numbers.begin(), numbers.end());

        auto it_winning_number = winning_numbers.begin();
        auto it_number = numbers.begin();

        while (it_winning_number != winning_numbers.end() &&
               it_number != numbers.end())
        {
            if (*it_winning_number == *it_number)
            {
                if (!points)
                    points = 1;
                else
                    points *= 2;
            }

            if (*it_winning_number < *it_number)
                it_winning_number++;
            else
                it_number++;
        }

        total_points += points;
    }

    return total_points;
}
