#include "main.h"

#include <bitset>
#include <sstream>

int Answer(std::ifstream &file)
{
    constexpr int max_number_of_bits = 12;
    int count[max_number_of_bits][2] = {};
    using DiagnosticType = std::bitset<max_number_of_bits>;
    DiagnosticType diagnostic;
    int number_of_bits = 0;
    std::string line;

    while (std::getline(file, line))
    {
        std::stringstream(line) >> diagnostic;

        if (number_of_bits == 0 && line.size() <= max_number_of_bits)
        {
            number_of_bits = line.size();
        }

        for (int bit = 0; bit < number_of_bits; bit++)
        {
            count[bit][diagnostic[bit]]++;
        }
    }

    DiagnosticType gamma;
    DiagnosticType epsilon;

    for (int bit = 0; bit < number_of_bits; bit++)
    {
        gamma[bit] = count[bit][1] >= count[bit][0];
        epsilon[bit] = count[bit][1] < count[bit][0];
    }

    return gamma.to_ulong() * epsilon.to_ulong();
}
