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
    if (vs.size() > 0)
    {
        int sx = vs[0].size();

        if (sx > 0)
        {
            std::stringstream border;
            border << '+' << std::string(sx, '-') << '+' << std::endl;
            std::cout << border.str();

            for (auto &row : vs)
            {
                std::cout << '|' << row << '|' << std::endl;
            }

            std::cout << border.str();
        }
    }
}

void DPRINT(const std::vector<std::vector<int>> &vi)
{
    std::vector<std::string> vs;
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
        std::stringstream vss;

        for (int i = 0; i < row.size(); i++)
        {
            if (size > 1)
            {
                if (i > 0)
                {
                    vss << " ";
                }

                std::stringstream ss;
                ss << row[i];
                int sp = size - ss.str().size();

                if (sp > 0)
                {
                    vss << std::string(sp, ' ');
                }
            }
            vss << row[i];
        }

        vs.push_back(vss.str());
    }

    DPRINT(vs);
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
