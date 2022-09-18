#include "main.h"

#include <bitset>

int Answer(std::ifstream &file)
{
    constexpr int number_of_bits = 12;
    int count[number_of_bits][2] = {};
    using DiagnosticType = std::bitset<number_of_bits>;
    DiagnosticType diagnostic;

    while (file >> diagnostic)
    {
        for (int bit = 0; bit < number_of_bits; bit++)
        {
            count[bit][diagnostic[bit]]++;
        }
    }

    DiagnosticType gamma;

    for (int bit = 0; bit < number_of_bits; bit++)
    {
        gamma[bit] = count[bit][1] > count[bit][0];
    }

    DiagnosticType epsilon = DiagnosticType(gamma).flip();

    return gamma.to_ulong() * epsilon.to_ulong();
}
