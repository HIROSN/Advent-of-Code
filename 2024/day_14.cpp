// #include <debug_print.h>
#include <main.h>
#include <skip.h>

#include <algorithm>
#include <map>
#include <sstream>
#include <utility>
#include <vector>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    struct Robot
    {
        int px;
        int py;
        int vx;
        int vy;
    };

    std::vector<Robot> robots;
    int size_x = 0, size_y = 0;
    std::string line;

    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        Robot robot;

        skip<char>(ss, 2);
        ss >> robot.px;
        skip<char>(ss);
        ss >> robot.py;

        skip<char>(ss, 2);
        ss >> robot.vx;
        skip<char>(ss);
        ss >> robot.vy;

        size_x = std::max(size_x, robot.px + 1);
        size_y = std::max(size_y, robot.py + 1);
        robots.push_back(robot);

        DPRINTX2(robot.px, robot.py);
        DPRINTX2(robot.vx, robot.vy);
        DPRINT_ENDL();
    }

    DPRINTX2(size_x, size_y);
    DPRINT_ENDL();

#ifdef DPRINT_ON
    auto debug_print = [&](const std::vector<Robot> &robots) -> void
    {
        std::map<std::pair<int, int>, char> mpc;
        for (const auto &robot : robots)
            if (mpc[{robot.px, robot.py}])
                mpc[{robot.px, robot.py}]++;
            else
                mpc[{robot.px, robot.py}] = '1';
        DPRINT3(mpc, '.', 1);
    };
    debug_print(robots);
#endif

    for (auto &robot : robots)
    {
        int px = robot.px + robot.vx * 100;
        int py = robot.py + robot.vy * 100;
        px = px % size_x;
        py = py % size_y;
        px += px < 0 ? size_x : 0;
        py += py < 0 ? size_y : 0;
        robot.px = px;
        robot.py = py;
    }

#ifdef DPRINT_ON
    debug_print(robots);
#endif

    int half_x = size_x / 2;
    int half_y = size_y / 2;
    int quadrants[4] = {};
    int safety_factor = 1;

    for (const auto &robot : robots)
    {
        if (robot.px == half_x || robot.py == half_y)
            continue;

        int index = robot.px < half_x ? 0 : 2;
        index += robot.py < half_y ? 0 : 1;
        quadrants[index]++;
    }

    for (const auto &quadrant : quadrants)
        safety_factor *= quadrant;

    return safety_factor;
}
