#include "main.h"

#include <algorithm>
#include <iterator>
#include <map>
#include <sstream>
#include <vector>

uint64_t Answer(std::ifstream &file)
{
    std::string line;
    std::getline(file, line);
    std::stringstream ss(line);
    std::vector<int> draw_numbers;

    while (ss.good())
    {
        std::string str;
        std::getline(ss, str, ',');
        int number;
        std::stringstream(str) >> number;
        draw_numbers.push_back(number);
    }

    constexpr int board_size = 5;
    constexpr int board_numbers = board_size * board_size;
    constexpr int number_of_lines = board_size + board_size;
    static std::map<int, std::vector<int>> lines;

    for (int i = 0; i < board_numbers; i++)
    {
        lines[i].push_back(i / board_size);
        lines[i].push_back((i % board_size) + board_size);
    }

    struct BoardType
    {
        std::vector<int> numbers;
        int marked[number_of_lines] = {};
        int score = 0;

        bool Bingo(int draw_number)
        {
            auto it = std::find(numbers.begin(), numbers.end(), draw_number);

            if (it != numbers.end())
            {
                score -= draw_number;
                int board_index = it - numbers.begin();

                for (auto line_index : lines[board_index])
                {
                    if (++marked[line_index] == board_size)
                    {
                        return true;
                    }
                }
            }
            return false;
        }
    };

    std::vector<BoardType> boards;

    while (file.good())
    {
        BoardType board;
        int number;

        for (int i = 0; i < board_numbers; i++)
        {
            if (file >> number)
            {
                board.numbers.push_back(number);
                board.score += number;
            }
        }

        if (board.numbers.size() == board_numbers)
        {
            boards.push_back(std::move(board));
        }
    }

    for (auto draw_number : draw_numbers)
    {
        for (auto it = boards.begin(); it != boards.end();)
        {
            if (it->Bingo(draw_number))
            {
                if (boards.size() == 1)
                {
                    return it->score * draw_number;
                }
                else
                {
                    boards.erase(it);
                }
            }
            else
            {
                it++;
            }
        }
    }

    return 0;
}
