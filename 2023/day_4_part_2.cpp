#include <main.h>

#include <algorithm>
#include <sstream>
#include <unordered_map>
#include <vector>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    std::string line;
    int card = 0;
    std::unordered_map<int, int> copies;

    while (std::getline(file, line))
    {
        card++;
        const auto colon = line.find(':');
        const auto bar = line.find('|');
        std::stringstream ss_left(line.substr(colon + 1, bar - colon - 1));
        std::stringstream ss_right(line.substr(bar + 1));
        std::vector<int> winning_numbers;
        std::vector<int> numbers;
        int number, matches = 0;

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
                matches++;

            if (*it_winning_number < *it_number)
                it_winning_number++;
            else
                it_number++;
        }

        copies[card]++;
        for (int i = card + 1; i <= card + matches; i++)
            copies[i] += copies[card];
    }

    int total_scratchcards = 0;
    for (auto it : copies)
        total_scratchcards += it.second;

    return total_scratchcards;
}
