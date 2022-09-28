#ifndef DPRINT_ON
#define DPRINT_ON

#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

void DPRINT_ENDL()
{
    std::cout << std::endl;
}

void DPRINT(const std::vector<std::string> &vs)
{
    for (auto &row : vs)
    {
        std::cout << row << std::endl;
    }
}

void DPRINT(const std::vector<std::vector<int>> &vi)
{
    int size = 0;

    for (auto &row : vi)
    {
        for (int i = 0; i < row.size(); i++)
        {
            std::stringstream ss;
            ss << row[i];
            int s = ss.str().size();

            if (s > size)
            {
                size = s;
            }
        }
    }

    for (auto &row : vi)
    {
        for (int i = 0; i < row.size(); i++)
        {
            if (size > 1)
            {
                if (i > 0)
                {
                    std::cout << " ";
                }

                std::stringstream ss;
                ss << row[i];
                int sp = size - ss.str().size();

                if (sp > 0)
                {
                    std::cout << std::string(sp, ' ');
                }
            }
            std::cout << row[i];
        }
        std::cout << std::endl;
    }
}

void DPRINT2(int x, int y)
{
    std::cout << "(" << x << "," << y << ")";
}

void DPRINT(const std::pair<int, int> &point)
{
    DPRINT2(point.first, point.second);
}

#endif // DPRINT_ON
