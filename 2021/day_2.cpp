#include "main.h"

uint64_t Answer(std::ifstream &file)
{
    std::string command;
    int units = 0;
    int position = 0;
    int depth = 0;

    while (file >> command >> units)
    {
        if (command == "forward")
        {
            position += units;
        }
        else if (command == "down")
        {
            depth += units;
        }
        else if (command == "up")
        {
            depth -= units;
        }
    }

    return position * depth;
}
