#include <main.h>

#include <queue>
#include <sstream>
#include <unordered_map>

using reverse_priority_queue = std::priority_queue<int, std::vector<int>,
                                                   std::greater<int>>;

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
        reverse_priority_queue winning_numbers;
        reverse_priority_queue numbers;
        int number, matches = 0;

        while (ss_left >> number)
            winning_numbers.push(number);
        while (ss_right >> number)
            numbers.push(number);

        while (!winning_numbers.empty() && !numbers.empty())
        {
            if (winning_numbers.top() == numbers.top())
                matches++;

            if (winning_numbers.top() < numbers.top())
                winning_numbers.pop();
            else
                numbers.pop();
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
