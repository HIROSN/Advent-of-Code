#include "main.h"

#include <climits>

uint64_t Answer(std::ifstream &file)
{
    int increased = 0;
    int depth = 0;
    int previous = INT_MAX;

    while (file >> depth)
    {
        if (depth > previous)
        {
            increased++;
        }
        previous = depth;
    }

    return increased;
}
