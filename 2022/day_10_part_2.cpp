#include <main.h>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    std::string instruction;
    int value = 0, executing = 0, x = 1;

    for (int cycle = 1; true; cycle++)
    {
        if (executing == 0)
        {
            if (instruction == "addx")
                x += value;

            if (!(file >> instruction))
                break;

            if (instruction == "addx")
            {
                file >> value;
                executing = 2;
            }
            else
            {
                executing = 1;
            }
        }

        int p = (cycle - 1) % 40;
        std::cout << (x - 1 <= p && p <= x + 1 ? '#' : '.');

        if (cycle % 40 == 0)
            std::cout << std::endl;

        executing--;
    }

    return {};
}
