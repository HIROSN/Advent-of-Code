#include <main.h>

#include <list>
#include <map>
#include <sstream>
#include <utility>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    struct InfiniteCorridors
    {
        using Key = std::pair<uint64_t, int>;
        std::map<Key, uint64_t> cache;

        bool is_even_digits(uint64_t number)
        {
            std::stringstream ss;
            ss << number;
            return !(ss.str().size() % 2);
        }

        std::pair<uint64_t, uint64_t> split(uint64_t number)
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
        }

        uint64_t explore(uint64_t stone, int blink)
        {
            Key key{stone, blink};
            if (cache.find(key) != cache.end())
                return cache[key];

            std::list<uint64_t> stones{stone};
            uint64_t total = 0;

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

            if (blink == 1)
            {
                total = stones.size();
            }
            else
            {
                for (auto it : stones)
                    total += explore(it, blink - 1);
            }

            cache[key] = total;
            return total;
        }
    };

    const int total_blinking = 75;
    InfiniteCorridors corridors;
    std::list<uint64_t> initial_arrangement;
    uint64_t number, total_stones = 0;

    while (file >> number)
        initial_arrangement.push_back(number);

    for (auto stone : initial_arrangement)
        total_stones += corridors.explore(stone, total_blinking);

    return total_stones;
}
