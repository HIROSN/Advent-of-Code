#include "main.h"

#include <algorithm>
#include <cctype>
#include <map>
#include <sstream>
#include <vector>

std::map<std::string, std::vector<std::string>> connected;
std::vector<std::string> paths;

struct PathInfo
{
    std::string path;
    std::map<std::string, int> small_caves_visited;
    int small_caves_visited_twice = 0;
};

void visit(const std::string &cave, PathInfo info);

uint64_t Answer(std::ifstream &file)
{
    std::string line;

    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string left, right;
        std::getline(ss, left, '-');
        ss >> right;

        if (left != "end" && right != "start")
        {
            connected[left].push_back(right);
        }

        if (right != "end" && left != "start")
        {
            connected[right].push_back(left);
        }
    }

    visit("start", {});

    return paths.size();
}

void visit(const std::string &cave, PathInfo info)
{
    if (!info.path.empty())
    {
        info.path += ",";
    }

    info.path += cave;

    if (cave == "end")
    {
        paths.push_back(info.path);
    }
    else if (!std::islower(cave[0]) ||
             ++info.small_caves_visited[cave] <= 1 ||
             ++info.small_caves_visited_twice <= 1)
    {
        for (auto cc : connected[cave])
        {
            visit(cc, info);
        }
    }
}
