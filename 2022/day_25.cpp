// #include <debug_print.h>
#include <main.h>

#include <cmath>
#include <map>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    using SNAFU = std::map<int, int>;

    std::map<char, int> digits = {
        {'2', 2},
        {'1', 1},
        {'0', 0},
        {'-', -1},
        {'=', -2},
    };

    auto to_snafu_digit = [&](int digit) -> char
    {
        for (const auto &map : digits)
            if (map.second == digit)
                return map.first;
        return '!';
    };

    auto to_integer = [](const SNAFU &snafu_number) -> int64_t
    {
        int64_t number = 0;
        for (const auto &map : snafu_number)
            number += std::pow(5, map.first) * map.second;
        return number;
    };

    int64_t sum_of_numbers = 0;
    std::string line;

    while (std::getline(file, line))
    {
        SNAFU snafu_number;
        for (auto it = line.rbegin(); it != line.rend(); it++)
            snafu_number[it - line.rbegin()] = digits[*it];
        sum_of_numbers += to_integer(snafu_number);
    }

    int size = 0;

    while (true)
    {
        const int64_t place = std::pow(5, size);
        auto divided = sum_of_numbers / place;
        if (divided)
            size++;
        else
            break;
    }

    DPRINT2(sum_of_numbers, size);
    DPRINT_ENDL();
    SNAFU snafu_number;

    auto print_snafu_number = [&](SNAFU &snafu_number) -> void
    {
        for (int i = size; i >= 0; i--)
        {
            if (snafu_number.find(i) != snafu_number.end())
                std::cout << to_snafu_digit(snafu_number[i]);
            else if (i < size)
                std::cout << ' ';
        }
        std::cout << std::endl;
    };

    for (int i = size - 1; i >= 0; i--)
    {
        const int64_t place = std::pow(5, i);
        auto remainder = sum_of_numbers - to_integer(snafu_number);
        snafu_number[i] = remainder / place;

        for (int j = i; j < size && snafu_number[j] > 2; j++)
        {
            snafu_number[j + 1]++;
            snafu_number[j] -= 5;
        }

#ifdef DPRINT_ON
        DPRINT2(i, remainder);
        print_snafu_number(snafu_number);
#endif
    }

    print_snafu_number(snafu_number);
    return {};
}
