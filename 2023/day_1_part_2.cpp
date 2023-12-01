#include <main.h>

#include <cctype>
#include <sstream>
#include <vector>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    int sum_of_calibration_values = 0;
    std::string line;

    const std::vector<std::string> spelled_numbers{"one", "two", "three",
                                                   "four", "five", "six",
                                                   "seven", "eight", "nine"};

    auto isdigit = [&](int i, bool reverse,
                       std::stringstream &two_digit) -> bool
    {
        if (std::isdigit(line[i]))
        {
            two_digit << line[i];
            return true;
        }

        for (int number = 1; number <= spelled_numbers.size(); number++)
        {
            const std::string &spell = spelled_numbers[number - 1];
            const int s = spell.size();

            if (reverse)
            {
                if (i - s + 1 >= 0 && line.substr(i - s + 1, s) == spell)
                {
                    two_digit << number;
                    return true;
                }
            }
            else
            {
                if (i + s < line.length() && line.substr(i, s) == spell)
                {
                    two_digit << number;
                    return true;
                }
            }
        }

        return false;
    };

    while (std::getline(file, line))
    {
        int calibration_value = 0;
        std::stringstream two_digit;

        for (int i = 0; i < line.length(); i++)
        {
            if (isdigit(i, false, two_digit))
                break;
        }

        for (int i = line.length() - 1; i >= 0; i--)
        {
            if (isdigit(i, true, two_digit))
                break;
        }

        two_digit >> calibration_value;
        sum_of_calibration_values += calibration_value;
    }

    return sum_of_calibration_values;
}
