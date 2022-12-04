#include <main.h>

#include <utility>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    std::pair<int, int> elf1, elf2;
    int overlap = 0;

    while (file >> elf1.first && file.get() == '-' && file >> elf1.second &&
           file.get() == ',' &&
           file >> elf2.first && file.get() == '-' && file >> elf2.second)
    {
        if (!(elf1.second < elf2.first ||
              elf2.second < elf1.first))
        {
            overlap++;
        }
    }

    return overlap;
}
