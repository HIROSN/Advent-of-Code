#include <main.h>

#include <algorithm>
#include <cmath>
#include <optional>
#include <vector>

char get_output_char(long long A)
{
    long long B = A % 8;              // 2,4,
    B = B ^ 5;                        // 1,5,
    long long C = A / std::pow(2, B); // 7,5,
    B = B ^ 6;                        // 1,6,
    B = B ^ C;                        // 4,1,
    char out = '0' + (B % 8);         // 5,5,
    return out;
}

std::optional<long long> find_A(
    int index, long long current, const std::string &output)
{
    const int total_digits = output.length();

    if (index == total_digits)
    {
        return current;
    }

    for (int octal_digit = 0; octal_digit <= 7; ++octal_digit)
    {
        long long potential = (current << 3) | octal_digit;

        if (get_output_char(potential) ==
            output[total_digits - index - 1])
        {
            if (auto result = find_A(index + 1, potential, output))
            {
                return result;
            }
        }
    }

    return std::nullopt;
}

std::optional<uint64_t> Answer(std::ifstream &file)
{
#if 0
    long long A = 60589763;
    std::string output;
    while (A)
    {
        auto ch = get_output_char(A);
        if (!output.empty())
            output += ",";
        output += ch;
        A = A / std::pow(2, 3); // 0,3,
    }
    std::cout << output;
#else
    if (auto final_A = find_A(0, 0, "2415751641550330"))
    {
        std::cout << *final_A << std::endl;
    }
#endif
    return {};
}
