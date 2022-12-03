#include <main.h>

#include <map>
#include <tuple>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    std::map<char, int> outcome = {
        {'X', 0},
        {'Y', 3},
        {'Z', 6},
    };

    std::map<std::tuple<char, int>, char> you = {
        {{'A', 3}, 'X'},
        {{'A', 6}, 'Y'},
        {{'A', 0}, 'Z'},
        {{'B', 0}, 'X'},
        {{'B', 3}, 'Y'},
        {{'B', 6}, 'Z'},
        {{'C', 6}, 'X'},
        {{'C', 0}, 'Y'},
        {{'C', 3}, 'Z'},
    };

    char opponent, end;
    int total_score = 0;

    while (file >> opponent && file >> end)
    {
        int shape = you[{opponent, outcome[end]}] - 'X' + 1;
        total_score += shape + outcome[end];
    }

    return total_score;
}
