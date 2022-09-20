#include "main.h"

#include <iterator>
#include <vector>

uint64_t Answer(std::ifstream &file)
{
    int increased = 0;
    std::vector<int> measurements(std::istream_iterator<int>(file), {});
    const int sliding_window_size = 3;

    if (measurements.size() > sliding_window_size)
    {
        for (size_t i = sliding_window_size; i < measurements.size(); i++)
        {
            if (measurements[i] > measurements[i - sliding_window_size])
            {
                increased++;
            }
        }
    }

    return increased;
}
