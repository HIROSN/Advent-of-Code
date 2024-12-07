#include <main.h>
#include <skip.h>

#include <sstream>
#include <vector>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    std::string line;
    uint64_t total_calibration_result = 0;

    while (std::getline(file, line))
    {
        std::stringstream ss(line);

        struct Evaluater
        {
            uint64_t test_value;
            std::vector<int> numbers;

            bool operate(uint64_t result, char op, size_t index)
            {
                if (index == numbers.size())
                    return result == test_value;

                if (op == '+')
                    result += numbers[index];
                else
                    result *= numbers[index];

                return operate(result, '*', index + 1) ||
                       operate(result, '+', index + 1);
            };
        };

        Evaluater eval;
        ss >> eval.test_value;
        skip<char>(ss);
        int number;
        while (ss >> number)
            eval.numbers.push_back(number);

        if (eval.operate(eval.numbers[0], '*', 1) ||
            eval.operate(eval.numbers[0], '+', 1))
        {
            total_calibration_result += eval.test_value;
        }
    }

    return total_calibration_result;
}
