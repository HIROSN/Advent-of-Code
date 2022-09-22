#include "main.h"

#include <algorithm>
#include <stack>
#include <vector>

uint64_t Answer(std::ifstream &file)
{
    const std::string open("([{<");
    const std::string close(")]}>");
    const int point[] = {3, 57, 1197, 25137};

    std::string line;
    std::vector<uint64_t> scores;

    while (file >> line)
    {
        std::stack<char> s;
        int error = 0;

        for (auto &ch : line)
        {
            if (open.find(ch) != std::string::npos)
            {
                s.push(ch);
                continue;
            }

            auto id = close.find(ch);

            if (id != std::string::npos)
            {
                if (s.top() == open[id])
                {
                    s.pop();
                    continue;
                }

                error = point[id];
                break;
            }
        }

        if (error == 0)
        {
            uint64_t total_score = 0;

            while (!s.empty())
            {
                total_score *= 5;
                total_score += open.find(s.top()) + 1;
                s.pop();
            }

            scores.push_back(total_score);
        }
    }

    if (scores.size() && (scores.size() % 2))
    {
        std::sort(scores.begin(), scores.end());
        return scores[scores.size() / 2];
    }

    return 0;
}
