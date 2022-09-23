#include "main.h"

#include <sstream>
#include <vector>

constexpr int days_after = 256;
constexpr int days_to_new_lanternfish = 7;

uint64_t new_lanternfish(int lifetime);

uint64_t Answer(std::ifstream &file)
{
    uint64_t lanternfish = 0;
    std::string str;

    while (std::getline(file, str, ','))
    {
        int timer;
        std::stringstream(str) >> timer;
        lanternfish++;
        int lifetime = days_after + days_to_new_lanternfish - timer + 1;
        lanternfish += new_lanternfish(lifetime);
    }

    return lanternfish;
}

uint64_t new_lanternfish(int lifetime)
{
    static uint64_t new_lanternfish_cache[days_after +
                                          days_to_new_lanternfish] = {};

    if (lifetime <= days_to_new_lanternfish + 1)
    {
        return 0;
    }

    if (new_lanternfish_cache[lifetime])
    {
        return new_lanternfish_cache[lifetime];
    }

    uint64_t lanternfish = 0;
    int new_lifetime = lifetime - 2;
    lanternfish += new_lifetime / days_to_new_lanternfish;

    for (new_lifetime -= days_to_new_lanternfish;
         new_lifetime > days_to_new_lanternfish + 1;
         new_lifetime -= days_to_new_lanternfish)
    {
        lanternfish += new_lanternfish(new_lifetime);
    }

    new_lanternfish_cache[lifetime] = lanternfish;
    return lanternfish;
}
