#include <main.h>

#include <list>
#include <sstream>
#include <vector>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    int sum_of_extrapolated_values = 0;
    std::string line;

    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::vector<int> sequence;
        std::list<int> beginning;
        bool all_zero = false;
        int extrapolated = 0, number;

        while (ss >> number)
            sequence.push_back(number);
        beginning.push_front(sequence[0]);

        for (int size = sequence.size(); !all_zero && size > 1; size--)
        {
            all_zero = true;

            for (int i = 1; i < size; i++)
            {
                int difference = sequence[i] - sequence[i - 1];
                sequence[i - 1] = difference;
                all_zero &= difference == 0;
            }

            beginning.push_front(sequence[0]);
        }

        for (const auto &value : beginning)
            extrapolated = value - extrapolated;

        sum_of_extrapolated_values += extrapolated;
    }

    return sum_of_extrapolated_values;
}
