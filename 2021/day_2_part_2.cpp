#include "main.h"

uint64_t Answer(std::ifstream &file)
{
    std::string command;
    int units = 0;
    int position = 0;
    int depth = 0;
    int aim = 0;

    while (file >> command >> units)
    {
        if (command == "forward")
        {
            position += units;
            depth += aim * units;
        }
        else if (command == "down")
        {
            aim += units;
        }
        else if (command == "up")
        {
            aim -= units;
        }
    }

    return position * depth;
}
