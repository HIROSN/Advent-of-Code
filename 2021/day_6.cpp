#include "main.h"

#include <sstream>

const int days_after = 80;
const int days_to_new_lanternfish = 7;

int NewLanternFish(int day, int timer);

uint64_t Answer(std::ifstream &file)
{
    int lanternfish = 0;

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

int NewLanternFish(int day, int timer)
{
    int lanternfish = 1;

    for (int new_day = day + timer + 1;
         new_day <= days_after;
         new_day += days_to_new_lanternfish)
    {
        lanternfish += NewLanternFish(new_day, days_to_new_lanternfish + 1);
    }

    return lanternfish;
}
