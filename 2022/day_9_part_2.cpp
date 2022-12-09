// #include <debug_print.h>
#include <main.h>

#include <cmath>
#include <set>
#include <utility>
#include <vector>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    char move;
    int steps;
    using Knot = std::pair<int, int>;
    constexpr int rope_size = 10;
    std::vector<Knot> rope(rope_size);
    Knot &head = rope[0];
    Knot &tail = rope[rope_size - 1];
    std::set<Knot> visited;

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

            for (int k = 1; k < rope_size; k++)
            {
                int moved_x = std::abs(rope[k - 1].first - rope[k].first);
                int moved_y = std::abs(rope[k - 1].second - rope[k].second);

                if (moved_x == 2)
                {
                    rope[k].first += rope[k - 1].first > rope[k].first ? 1 : -1;

                    if (moved_y < 2 && rope[k].second != rope[k - 1].second)
                    {
                        rope[k].second = rope[k - 1].second;
                    }
                }

                if (moved_y == 2)
                {
                    rope[k].second += rope[k - 1].second > rope[k].second ? 1 : -1;

                    if (moved_x < 2 && rope[k].first != rope[k - 1].first)
                    {
                        rope[k].first = rope[k - 1].first;
                    }
                }
            }

            if (visited.find(tail) == visited.end())
            {
                visited.insert(tail);
            }
#ifdef DPRINT_ON
            for (auto &knot : rope)
                DPRINT(knot);
            DPRINT_ENDL();
#endif
        }
    }

    return visited.size();
}
