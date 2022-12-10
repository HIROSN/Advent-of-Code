#include <main.h>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    std::string instruction;
    int value = 0, executing = 0, register_x = 1, sum_of_signal_strengths = 0;

    for (int cycle = 1; true; cycle++)
    {
        if (executing == 0)
        {
            if (instruction == "addx")
                register_x += value;

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

        if (cycle % 20 == 0 && cycle / 20 % 2 == 1)
        {
            sum_of_signal_strengths += cycle * register_x;
        }

        executing--;
    }

    return sum_of_signal_strengths;
}
