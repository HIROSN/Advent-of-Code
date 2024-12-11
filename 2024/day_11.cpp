// #include <debug_print.h>
#include <main.h>

#include <list>
#include <sstream>
#include <utility>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    std::list<uint64_t> stones;
    uint64_t number;

    while (file >> number)
        stones.push_back(number);

    auto is_even_digits = [](uint64_t number) -> bool
    {
        std::stringstream ss;
        ss << number;
        return !(ss.str().size() % 2);
    };

    auto split = [](uint64_t number) -> std::pair<uint64_t, uint64_t>
    {
        std::stringstream ss;
        ss << number;
        size_t len = ss.str().size();
        auto str_left = ss.str().substr(0, len / 2);
        auto str_right = ss.str().substr(len / 2);
        std::stringstream ss_left(str_left);
        std::stringstream ss_right(str_right);
        uint64_t left, right;
        ss_left >> left;
        ss_right >> right;
        return {left, right};
    };

    for (int blink = 1; blink <= 25; blink++)
    {
        for (auto it = stones.begin(); it != stones.end(); it++)
        {
            if (*it == 0)
            {
                *it = 1;
            }
            else if (is_even_digits(*it))
            {
                auto two_stones = split(*it);
                it = stones.insert(it, two_stones.first);
                *++it = two_stones.second;
            }
            else
            {
                *it *= 2024;
            }
        }

        for (auto stone : stones)
            DPRINTX(stone << " ");
        DPRINT_ENDL();
    }

    return stones.size();
}
