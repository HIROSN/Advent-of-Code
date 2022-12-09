#include <main.h>

#include <cmath>
#include <set>
#include <utility>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    char move;
    int steps;
    std::pair<int, int> head = {0, 0};
    auto tail = head;
    std::set<std::pair<int, int>> visited;

    while (file >> move >> steps)
    {
        for (int i = 0; i < steps; i++)
        {
            switch (move)
            {
            case 'D':
                head.second--;
                break;
            case 'L':
                head.first--;
                break;
            case 'R':
                head.first++;
                break;
            case 'U':
                head.second++;
                break;
            }

            int moved_x = std::abs(head.first - tail.first);
            int moved_y = std::abs(head.second - tail.second);

            if (moved_x == 2)
            {
                tail.first += head.first > tail.first ? 1 : -1;

                if (tail.second != head.second)
                {
                    tail.second = head.second;
                }
            }

            if (moved_y == 2)
            {
                tail.second += head.second > tail.second ? 1 : -1;

                if (tail.first != head.first)
                {
                    tail.first = head.first;
                }
            }

            if (visited.find(tail) == visited.end())
            {
                visited.insert(tail);
            }
        }
    }

    return visited.size();
}
