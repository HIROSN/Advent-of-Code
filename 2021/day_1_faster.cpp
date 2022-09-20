#include "main.h"

#include <climits>
#include <iterator>
#include <vector>

uint64_t Answer(std::ifstream &file)
{
    int increased = 0;
    int previous = INT_MAX;
    std::vector<int> measurements(std::istream_iterator<int>(file), {});

    for (auto depth : measurements)
    {
        if (depth > previous)
        {
            increased++;
        }
        previous = depth;
    }

    return increased;
}
