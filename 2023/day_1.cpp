#include <main.h>

#include <cctype>
#include <sstream>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    int sum_of_calibration_values = 0;
    std::string line;

    while (std::getline(file, line))
    {
        int calibration_value = 0;
        std::string two_digit;

        for (auto it = line.begin(); it != line.end(); it++)
        {
            if (std::isdigit(*it))
            {
                two_digit += *it;
                break;
            }
        }

        for (auto it = line.rbegin(); it != line.rend(); it++)
        {
            if (std::isdigit(*it))
            {
                two_digit += *it;
                break;
            }
        }

        std::stringstream(two_digit) >> calibration_value;
        sum_of_calibration_values += calibration_value;
    }

    return sum_of_calibration_values;
}
