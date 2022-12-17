// #include <debug_print.h>
#include <main.h>
#include <skip.h>

#include <map>
#include <utility>

constexpr int limit = 4000000;
constexpr int block = 1000000;
using Point = std::pair<int, int>;

bool read_point(std::ifstream &file, Point &point);
bool read_integer(std::ifstream &file, int &integer);
int distance(const Point &a, const Point &b);
bool find(const std::map<Point, int> &reports,
          int sx, int sy, int ex, int ey, int b, Point &distress_beacon);

std::optional<uint64_t> Answer(std::ifstream &file)
{
    std::map<Point, int> reports;
    Point sensor, beacon, distress_beacon;

    while (read_point(file, sensor) && read_point(file, beacon))
        reports[sensor] = distance(sensor, beacon);

    if (find(reports, 0, 0, limit, limit, block, distress_beacon))
    {
        uint64_t tuning_frequency = distress_beacon.first;
        tuning_frequency *= limit;
        tuning_frequency += distress_beacon.second;
        DPRINT(distress_beacon);
        DPRINT_ENDL();
        return tuning_frequency;
    }

    return {};
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

int distance(const Point &a, const Point &b)
{
    return std::abs(a.first - b.first) + std::abs(a.second - b.second);
}

bool find(const std::map<Point, int> &reports,
          int sx, int sy, int ex, int ey, int b, Point &distress_beacon)
{
    for (int x = sx; x < ex; x += b)
    {
        for (int y = sy; y < ey; y += b)
        {
            struct Covered
            {
                bool top_left;
                bool top_right;
                bool bottom_left;
                bool bottom_right;

                bool fully(int block)
                {
                    if (block > 1)
                        return top_left && bottom_right ||
                               top_right && bottom_left;
                    return top_left;
                }
            };
            Covered half_covered = {};

            for (const auto &report : reports)
            {
                const Point &p = report.first;
                const int &d = report.second;

                if (b > 1)
                {
                    Covered c;
                    c.top_left = distance({x, y}, p) <= d;
                    c.top_right = distance({x + b, y}, p) <= d;
                    c.bottom_left = distance({x, y + b}, p) <= d;
                    c.bottom_right = distance({x + b, y + b}, p) <= d;

                    if (c.top_left && c.top_right && c.bottom_left)
                        half_covered.top_left = true;
                    if (c.top_left && c.top_right && c.bottom_right)
                        half_covered.top_right = true;
                    if (c.bottom_left && c.bottom_right && c.top_left)
                        half_covered.bottom_left = true;
                    if (c.bottom_left && c.bottom_right && c.top_right)
                        half_covered.bottom_right = true;
                }
                else
                {
                    half_covered.top_left = distance({x, y}, p) <= d;
                }

                if (half_covered.fully(b))
                    break;
            }

            if (!half_covered.fully(b))
            {
                if (b > 1)
                {
#ifdef DPRINT_ON
                    if (b > 1000)
                        DPRINT2(x, y);
#endif
                    int bex = x + b;
                    int bey = y + b;
                    int sb = b / 10;
                    if (sb == 1)
                    {
                        if (bex == limit)
                            bex++;
                        if (bey == limit)
                            bey++;
                    }
                    if (find(reports, x, y, bex, bey, sb, distress_beacon))
                        return true;
                }
                else
                {
                    distress_beacon = {x, y};
                    return true;
                }
            }
        }
    }

    return false;
}
