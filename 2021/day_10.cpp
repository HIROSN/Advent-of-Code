#include "main.h"

#include <stack>

uint64_t Answer(std::ifstream &file)
{
    const std::string open("([{<");
    const std::string close(")]}>");
    const int point[] = {3, 57, 1197, 25137};

    std::string line;
    int total_score = 0;

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

        total_score += error;
    }

    return total_score;
}
