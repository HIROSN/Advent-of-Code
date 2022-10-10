#include "main.h"

#include <algorithm>
#include <queue>

struct Player
{
    int id = 0;
    int position = 0;
    int score = 0;
    uint64_t universes = 1;

    void move(int by)
    {
        position += by;
        position -= position > 10 ? 10 : 0;
        score += position;
    }

    bool wins()
    {
        return score >= 21;
    }

    Player(std::ifstream &file)
    {
        std::string line;
        std::getline(file, line);
        static const char format[] = "Player %d starting position: %d";
        sscanf(line.c_str(), format, &id, &position);
    }
};

struct Game
{
    Player first;
    Player second;
};

uint64_t Answer(std::ifstream &file)
{
    const int moves_to_universes[] = {1, 3, 6, 7, 6, 3, 1};
    std::queue<Game> games;
    games.push({Player(file), Player(file)});
    uint64_t total_player_universes[2] = {};

    while (true)
    {
        auto &game = games.front();

        for (int move_1 = 3; move_1 <= 9; move_1++)
        {
            auto player = game.first;
            player.move(move_1);
            player.universes *= moves_to_universes[move_1 - 3];

            if (player.wins())
            {
                total_player_universes[player.id - 1] += player.universes;
                continue;
            }

            for (int move_2 = 3; move_2 <= 9; move_2++)
            {
                auto player_1 = player;
                player_1.universes *= moves_to_universes[move_2 - 3];

                auto player_2 = game.second;
                player_2.move(move_2);
                player_2.universes *= moves_to_universes[move_1 - 3];
                player_2.universes *= moves_to_universes[move_2 - 3];

                if (player_2.wins())
                {
                    total_player_universes[player_2.id - 1] += player_2.universes;
                    continue;
                }

                games.push({player_1, player_2});
            }
        }

        games.pop();

        if (games.empty())
        {
            break;
        }
    }

    uint64_t wins_more = 0;

    for (auto &universes : total_player_universes)
    {
        wins_more = std::max(wins_more, universes);
    }

    return wins_more;
}
