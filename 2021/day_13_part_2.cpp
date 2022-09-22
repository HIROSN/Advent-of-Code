#include "main.h"

#include <algorithm>
#include <map>
#include <sstream>
#include <utility>
#include <vector>

#define X 0
#define Y 1

uint64_t Answer(std::ifstream &file)
{
    std::string line, fold, along;
    std::vector<std::pair<int, int>> coordinates;
    std::vector<std::pair<std::string, int>> instructions;

    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string left, right;

        if (std::getline(ss, left, ',') &&
            std::getline(ss, right, ','))
        {
            int x, y;
            std::stringstream(left) >> x;
            std::stringstream(right) >> y;
            coordinates.push_back(std::make_pair(x, y));
            continue;
        }
        break;
    }

    while (file >> fold >> along >> line)
    {
        std::stringstream ss(line);
        std::string xy;
        int at;

        if (std::getline(ss, xy, '=') &&
            ss >> at)
        {
            instructions.push_back(std::make_pair(xy, at));
        }
    }

    int size_x = 0;
    int size_y = 0;
    std::map<int, std::map<int, bool>> paper;

    for (auto c : coordinates)
    {
        int x = std::get<X>(c);
        int y = std::get<Y>(c);
        size_x = std::max(size_x, x + 1);
        size_y = std::max(size_y, y + 1);
        paper[y][x] = true;
    }

    for (auto i : instructions)
    {
        int fold_at = std::get<int>(i);

        switch (std::get<std::string>(i)[0])
        {
        case 'x':
            for (int y = 0; y < size_y; y++)
            {
                for (int x = 0; x < size_x / 2; x++)
                {
                    paper[y][x] |= paper[y][size_x - x - 1];
                }
            }
            size_x /= 2;
            break;
        case 'y':
            for (int x = 0; x < size_x; x++)
            {
                for (int y = 0; y < size_y / 2; y++)
                {
                    paper[y][x] |= paper[size_y - y - 1][x];
                }
            }
            size_y /= 2;
            break;
        }
    }

    for (int y = 0; y < size_y; y++)
    {
        for (int x = 0; x < size_x; x++)
        {
            std::cout << (paper[y][x] ? '#' : ' ');
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;

    return 0;
}
