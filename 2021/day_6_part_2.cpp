#include "main.h"

#include <sstream>

const int days_after = 256;
const int days_to_new_lanternfish = 7;

uint64_t NewLanternFish(int day, int timer);

uint64_t Answer(std::ifstream &file)
{
    uint64_t lanternfish = 0;

    while (file.good())
    {
        std::string str;
        std::getline(file, str, ',');
        int timer;
        std::stringstream(str) >> timer;
        lanternfish += NewLanternFish(0, timer);
    }

    return lanternfish;
}

uint64_t NewLanternFish(int day, int timer)
{
    uint64_t lanternfish = 1;

    for (int new_day = day + timer + 1;
         new_day <= days_after;
         new_day += days_to_new_lanternfish)
    {
        lanternfish += NewLanternFish(new_day, days_to_new_lanternfish + 1);
    }

    return lanternfish;
}
