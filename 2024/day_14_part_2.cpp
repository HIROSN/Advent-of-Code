// #include <debug_print.h>
#include <main.h>
#include <skip.h>

#include <algorithm>
#include <map>
#include <queue>
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

    std::vector<Robot> initial_state;
    int size_x = 0, size_y = 0, seconds = 0;
    std::map<int, int> seconds_peak_map;
    int highest_peak_size = 0;
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
        initial_state.push_back(robot);
    }

    auto elapsed = [&](int seconds,
                       std::vector<Robot> robots) -> std::vector<Robot>
    {
        for (auto &robot : robots)
        {
            int px = robot.px + robot.vx * seconds;
            int py = robot.py + robot.vy * seconds;
            px = px % size_x;
            py = py % size_y;
            px += px < 0 ? size_x : 0;
            py += py < 0 ? size_y : 0;
            robot.px = px;
            robot.py = py;
        }
        return robots;
    };

    auto histogram_peak = [](const std::vector<Robot> &robots)
        -> std::pair<int, int>
    {
        std::map<int, int> histogram_x, histogram_y;
        std::priority_queue<int> peak_x, peak_y;

        for (const auto &robot : robots)
        {
            histogram_x[robot.px]++;
            histogram_y[robot.py]++;
        }

        for (auto it : histogram_x)
            peak_x.push(it.second);
        for (auto it : histogram_y)
            peak_y.push(it.second);

        return {peak_x.top(), peak_y.top()};
    };

    while (++seconds < size_x * size_y)
    {
        auto robots = elapsed(seconds, initial_state);
        auto peak = histogram_peak(robots);
        int peak_size = peak.first * peak.second;
        seconds_peak_map[seconds] = peak_size;

        if (peak_size > highest_peak_size)
            highest_peak_size = peak_size;
    }

    for (int seconds = 1; seconds < size_x * size_y; seconds++)
    {
        if (seconds_peak_map[seconds] == highest_peak_size)
        {
#ifdef DPRINT_ON
            auto robots = elapsed(seconds, initial_state);
            std::map<std::pair<int, int>, char> mpc;
            for (const auto &robot : robots)
                if (mpc[{robot.px, robot.py}])
                    mpc[{robot.px, robot.py}]++;
                else
                    mpc[{robot.px, robot.py}] = '1';
            DPRINT3(mpc, '.', 1);
#endif
            return seconds;
        }
    }

    return {};
}
