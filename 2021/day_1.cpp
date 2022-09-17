#include "main.h"

#include <climits>
#include <sstream>

int Answer(std::ifstream &file)
{
    int increased = 0;
    int previous = INT_MAX;
    std::string line;

    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        int depth;
        ss >> depth;
        if (depth > previous)
        {
            increased++;
        }
        previous = depth;
    }

    return increased;
}
