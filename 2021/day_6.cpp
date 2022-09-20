#include "main.h"

#include <sstream>
#include <vector>

const int days_after = 80;
const int days_to_new_lanternfish = 7;

uint64_t Answer(std::ifstream &file)
{
    std::vector<int> lanternfish;

    while (file.good())
    {
        std::string str;
        std::getline(file, str, ',');
        int timer;
        std::stringstream(str) >> timer;
        lanternfish.push_back(timer);
    }

    for (int day = 1; day <= days_after; day++)
    {
        int new_lanternfish = 0;

        for (auto &timer : lanternfish)
        {
            if (--timer < 0)
            {
                timer = days_to_new_lanternfish - 1;
                new_lanternfish++;
            }
        }

        for (int i = 0; i < new_lanternfish; i++)
        {
            lanternfish.push_back(days_to_new_lanternfish + 1);
        }
    }

    return lanternfish.size();
}
