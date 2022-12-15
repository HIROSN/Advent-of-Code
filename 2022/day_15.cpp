// #include <debug_print.h>
#include <main.h>
#include <skip.h>

#include <algorithm>
#include <cmath>
#include <map>
#include <utility>
#include <vector>

using Point = std::pair<int, int>;

bool read_point(std::ifstream &file, Point &point);
bool read_integer(std::ifstream &file, int &integer);
int manhattan_distance(const Point &a, const Point &b);

std::optional<uint64_t> Answer(std::ifstream &file)
{
    struct Report
    {
        Point sensor;
        Point closest_beacon;
        int distance;
    };

    std::vector<Report> reports;
    std::map<Point, char> map;
    int left, right;

    while (true)
    {
        Report report;
        auto &sensor = report.sensor;
        auto &beacon = report.closest_beacon;
        auto &distance = report.distance;
        if (read_point(file, sensor) && read_point(file, beacon))
        {
            distance = manhattan_distance(sensor, beacon);
            if (map.empty())
            {
                left = sensor.first;
                right = sensor.first;
            }
            else
            {
                left = std::min(left, sensor.first - distance);
                right = std::max(right, sensor.first + distance);
            }
            map[sensor] = 'S';
            map[beacon] = 'B';
            reports.push_back(std::move(report));
            continue;
        }
        break;
    }
    DPRINT3(map, '.', 1);
    DPRINT2(left, right);
    DPRINT_ENDL();

#ifdef DPRINT_ON
    const int row = 10;
#else
    const int row = 2000000;
#endif
    int positions_cannot_contain_beacon = 0;
    for (int x = left; x <= right; x++)
    {
        Point p = {x, row};
        if (map.find(p) != map.end())
            continue;

        for (const auto &report : reports)
        {
            if (manhattan_distance(p, report.sensor) <= report.distance)
            {
                positions_cannot_contain_beacon++;
                break;
            }
        }
    }

    return positions_cannot_contain_beacon;
}

bool read_point(std::ifstream &file, Point &point)
{
    return read_integer(file, point.first) &&
           read_integer(file, point.second);
}

bool read_integer(std::ifstream &file, int &integer)
{
    skip(file, '=');
    return static_cast<bool>(file >> integer);
}

int manhattan_distance(const Point &a, const Point &b)
{
    return std::abs(a.first - b.first) + std::abs(a.second - b.second);
}
