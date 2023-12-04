#include <main.h>

#include <queue>
#include <sstream>

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
        std::priority_queue<int> winning_numbers;
        std::priority_queue<int> numbers;
        int number, points = 0;

        while (ss_left >> number)
            winning_numbers.push(number);
        while (ss_right >> number)
            numbers.push(number);

        while (!winning_numbers.empty() && !numbers.empty())
        {
            if (winning_numbers.top() == numbers.top())
            {
                if (!points)
                    points = 1;
                else
                    points *= 2;
            }

            if (winning_numbers.top() > numbers.top())
                winning_numbers.pop();
            else
                numbers.pop();
        }

        total_points += points;
    }

    return total_points;
}
