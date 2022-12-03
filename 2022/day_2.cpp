#include <main.h>

#include <map>
#include <utility>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    std::map<std::pair<char, char>, int> outcome = {
        {{'A', 'X'}, 3},
        {{'A', 'Y'}, 6},
        {{'A', 'Z'}, 0},
        {{'B', 'X'}, 0},
        {{'B', 'Y'}, 3},
        {{'B', 'Z'}, 6},
        {{'C', 'X'}, 6},
        {{'C', 'Y'}, 0},
        {{'C', 'Z'}, 3},
    };

    char opponent, you;
    int total_score = 0;

    while (file >> opponent && file >> you)
    {
        int shape = you - 'X' + 1;
        total_score += shape + outcome[{opponent, you}];
    }

    return total_score;
}
