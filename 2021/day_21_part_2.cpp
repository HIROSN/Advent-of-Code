#include "main.h"

#include <algorithm>
#include <queue>
#include <unordered_map>
#include <utility>

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

using Game = std::pair<Player, Player>;

std::unordered_map<int, int> moves_to_universes;

uint64_t Answer(std::ifstream &file)
{
    for (int move_1 = 1; move_1 <= 3; move_1++)
        for (int move_2 = 1; move_2 <= 3; move_2++)
            for (int move_3 = 1; move_3 <= 3; move_3++)
                moves_to_universes[move_1 + move_2 + move_3]++;

    std::queue<Game> games;
    games.push({Player(file), Player(file)});
    std::unordered_map<int, uint64_t> total_player_universes;

    while (true)
    {
        auto &game = games.front();

        for (auto &move_1 : moves_to_universes)
        {
            auto player = game.first;
            player.move(move_1.first);
            player.universes *= moves_to_universes[move_1.first];

            if (player.wins())
            {
                total_player_universes[player.id] += player.universes;
                continue;
            }

            for (auto &move_2 : moves_to_universes)
            {
                auto player_1 = player;
                player_1.universes *= moves_to_universes[move_2.first];

                auto player_2 = game.second;
                player_2.move(move_2.first);
                player_2.universes *= moves_to_universes[move_1.first];
                player_2.universes *= moves_to_universes[move_2.first];

                if (player_2.wins())
                {
                    total_player_universes[player_2.id] += player_2.universes;
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

    for (auto &it : total_player_universes)
    {
        wins_more = std::max(wins_more, it.second);
    }

    return wins_more;
}
