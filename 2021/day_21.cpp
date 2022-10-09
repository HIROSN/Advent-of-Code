#include "main.h"

#include <algorithm>

uint64_t Answer(std::ifstream &file)
{
    struct DeterministicDice
    {
        int rolled = 0;

        int roll()
        {
            return ((++rolled - 1) % 100) + 1;
        }
    };

    struct Player
    {
        int id = 0;
        int position = 0;
        int score = 0;

        bool move(int by)
        {
            position = ((position + by - 1) % 10) + 1;
            score += position;
            return score < 1000;
        }

        Player(std::ifstream &file)
        {
            std::string line;
            std::getline(file, line);
            sscanf(line.c_str(),
                   "Player %d starting position: %d",
                   &id, &position);
        }
    };

    DeterministicDice die;
    Player player_1(file);
    Player player_2(file);

    while (player_1.move(die.roll() + die.roll() + die.roll()) &&
           player_2.move(die.roll() + die.roll() + die.roll()))
    {
    }

    return std::min(player_1.score, player_2.score) * die.rolled;
}
