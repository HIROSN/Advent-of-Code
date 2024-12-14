#include <debug_print.h>
#undef DTIMER_ON
#include <main.h>
#include <skip.h>

#include <algorithm>
#include <list>
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

    struct Histograms
    {
        using Histogram = std::map<int, int>;

        Histogram x;
        Histogram y;

        int top(int rank, const Histogram &histogram)
        {
            std::priority_queue<int> peak;

            for (auto it : histogram)
                peak.push(it.second);

            for (int i = 0; i < rank - 1; i++)
                peak.pop();

            return peak.top();
        }
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

    auto get_histograms = [](const std::vector<Robot> &robots) -> Histograms
    {
        Histograms histograms;

        for (const auto &robot : robots)
        {
            histograms.x[robot.px]++;
            histograms.y[robot.py]++;
        }

        return histograms;
    };

    while (++seconds < size_x * size_y)
    {
        auto robots = elapsed(seconds, initial_state);
        auto histograms = get_histograms(robots);
        int peak_size = histograms.top(1, histograms.x) *
                        histograms.top(1, histograms.y);
        seconds_peak_map[seconds] = peak_size;

        if (peak_size > highest_peak_size)
            highest_peak_size = peak_size;
    }

    for (int seconds = 1; seconds < size_x * size_y; seconds++)
    {
        if (seconds_peak_map[seconds] == highest_peak_size)
        {
            auto robots = elapsed(seconds, initial_state);
            auto histograms = get_histograms(robots);

            using Peak = std::pair<int, int>;
            Peak peak_x{histograms.top(1, histograms.x),
                        histograms.top(2, histograms.x)};
            Peak peak_y{histograms.top(1, histograms.y),
                        histograms.top(2, histograms.y)};

            std::list<int> frame_x, frame_y;
            std::map<std::pair<int, int>, char> tree;

            for (auto it : histograms.x)
                if (it.second == peak_x.first || it.second == peak_x.second)
                    frame_x.push_back(it.first);
            CHECK(frame_x.size() == 2);

            for (auto it : histograms.y)
                if (it.second == peak_y.first || it.second == peak_y.second)
                    frame_y.push_back(it.first);
            CHECK(frame_y.size() == 2);

            int start_x = *std::min_element(frame_x.begin(), frame_x.end());
            int end_x = *std::max_element(frame_x.begin(), frame_x.end());
            int start_y = *std::min_element(frame_y.begin(), frame_y.end());
            int end_y = *std::max_element(frame_y.begin(), frame_y.end());

            for (const auto &robot : robots)
            {
                const int &x = robot.px;
                const int &y = robot.py;

                if (x > start_x && x < end_x && y > start_y && y < end_y)
                {
                    if (tree[{x, y}])
                        tree[{x, y}]++;
                    else
                        tree[{x, y}] = '1';
                }
            }

            DPRINT3(tree, '.', 1);
            return seconds;
        }
    }

    return {};
}
