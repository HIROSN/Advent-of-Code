#ifndef DPRINT_ON
#define DPRINT_ON

#include <algorithm>
#include <climits>
#include <iostream>
#include <map>
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

std::vector<std::vector<int>> mpi_to_vi(
    const std::map<std::pair<int, int>, int> &mpi, int def, int dy)
{
    int sx = INT_MAX;
    int sy = INT_MAX;
    int ex = INT_MIN;
    int ey = INT_MIN;

    for (auto &p : mpi)
    {
        sx = std::min(sx, p.first.first);
        sy = std::min(sy, p.first.second);
        ex = std::max(ex, p.first.first);
        ey = std::max(ey, p.first.second);
    }

    if (dy < 0)
        std::swap(sy, ey);

    std::vector<std::vector<int>> vi;

    for (int y = sy; true; y += dy)
    {
        std::vector<int> line;

        for (int x = sx; x <= ex; x++)
        {
            auto it = mpi.find(std::make_pair(x, y));
            line.push_back(it != mpi.end() ? it->second : def);
        }

        vi.push_back(line);
        if (y == ey)
            break;
    }

    return vi;
}

std::vector<std::string> mpc_to_vs(
    const std::map<std::pair<int, int>, char> &mpc, char ch, int dy)
{
    int sx = INT_MAX;
    int sy = INT_MAX;
    int ex = INT_MIN;
    int ey = INT_MIN;

    for (auto &p : mpc)
    {
        sx = std::min(sx, p.first.first);
        sy = std::min(sy, p.first.second);
        ex = std::max(ex, p.first.first);
        ey = std::max(ey, p.first.second);
    }

    if (dy < 0)
        std::swap(sy, ey);

    std::vector<std::string> vs;

    for (int y = sy; true; y += dy)
    {
        std::string line;

        for (int x = sx; x <= ex; x++)
        {
            auto it = mpc.find(std::make_pair(x, y));
            line += (it != mpc.end() ? it->second : ch);
        }

        vs.push_back(line);
        if (y == ey)
            break;
    }

    return vs;
}

void DPRINT3(const std::map<std::pair<int, int>, char> &mpc, char ch, int dy)
{
    DPRINT(mpc_to_vs(mpc, ch, dy));
}

void DPRINT2(const std::map<std::pair<int, int>, char> &mpc, char ch)
{
    DPRINT3(mpc, ch, -1);
}

void DPRINT2(int x, int y)
{
    std::cout << "(" << x << "," << y << ")";
}

void DPRINT(const std::pair<int, int> &point)
{
    DPRINT2(point.first, point.second);
}

void DPRINT3(int x, int y, int z)
{
    std::cout << x << "," << y << "," << z;
}

#define DPRINTX(any) std::cout << any;

#define DPRINTX_ENDL(any) std::cout << any << std::endl;

#endif // DPRINT_ON
