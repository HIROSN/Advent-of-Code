#include "main.h"

#include <sstream>
#include <queue>

uint64_t Answer(std::ifstream &file)
{
    int increased = 0;
    std::queue<int> measurements;
    std::string line;
    const int sliding_window_size = 3;

    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        int depth;
        ss >> depth;
        measurements.push(depth);

        if (measurements.size() > sliding_window_size)
        {
            if (depth > measurements.front())
            {
                increased++;
            }
            measurements.pop();
        }
    }

    return increased;
}
