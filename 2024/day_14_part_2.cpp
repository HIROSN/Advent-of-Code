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
        using TopTwo = std::pair<int, int>;

        Histogram x;
        Histogram y;

        TopTwo top2(const Histogram &histogram)
        {
            std::priority_queue<int> peak;

            for (auto it : histogram)
                peak.push(it.second);

            int top = peak.top();
            peak.pop();
            return {top, peak.top()};
        }
    };

    using Robots = std::vector<Robot>;
    Robots initial_state;
    int size_x = 0, size_y = 0, fewest_seconds = 0;
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

    auto elapsed = [&](int seconds, Robots robots) -> Robots
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

    auto get_histograms = [](const Robots &robots) -> Histograms
    {
        Histograms histograms;

        for (const auto &robot : robots)
        {
            histograms.x[robot.px]++;
            histograms.y[robot.py]++;
        }

        return histograms;
    };

    while (++fewest_seconds < size_x * size_y)
    {
        auto robots = elapsed(fewest_seconds, initial_state);
        auto histograms = get_histograms(robots);
        auto top2_x = histograms.top2(histograms.x);
        auto top2_y = histograms.top2(histograms.y);

        std::list<int> frame_x, frame_y;
        std::map<std::pair<int, int>, char> tree;
        bool has_frame = true;

        for (auto it : histograms.x)
            if (it.second == top2_x.first || it.second == top2_x.second)
                frame_x.push_back(it.first);

        for (auto it : histograms.y)
            if (it.second == top2_y.first || it.second == top2_y.second)
                frame_y.push_back(it.first);

        if (frame_x.size() != 2 || frame_y.size() != 2)
            continue;

        int start_x = *std::min_element(frame_x.begin(), frame_x.end());
        int end_x = *std::max_element(frame_x.begin(), frame_x.end());
        int start_y = *std::min_element(frame_y.begin(), frame_y.end());
        int end_y = *std::max_element(frame_y.begin(), frame_y.end());

        for (const auto &robot : robots)
        {
            const int &x = robot.px;
            const int &y = robot.py;

            if (x >= start_x && x <= end_x && y >= start_y && y <= end_y)
            {
                if (tree[{x, y}])
                    tree[{x, y}]++;
                else
                    tree[{x, y}] = '1';
            }
        }

        for (int x = start_x; has_frame && x <= end_x; x++)
            has_frame = tree.find({x, start_y}) != tree.end() &&
                        tree.find({x, end_y}) != tree.end();

        for (int y = start_y; has_frame && y <= end_y; y++)
            has_frame = tree.find({start_x, y}) != tree.end() &&
                        tree.find({end_x, y}) != tree.end();

        if (!has_frame)
            continue;

        for (auto row : mpc_to_vs(tree, '.', 1))
            std::cout << row << std::endl;

        return fewest_seconds;
    }

    return {};
}
