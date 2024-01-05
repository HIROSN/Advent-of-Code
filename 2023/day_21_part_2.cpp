// #define PART_1
// #include <debug_print.h>
#include <main.h>

#include <algorithm>
#include <map>
#include <utility>
#include <vector>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    using Point = std::pair<int64_t, int64_t>;
    int64_t size_x, size_y;
    Point start, infinite_x;

#ifdef DPRINT_ON
    std::map<std::pair<int, int>, char> print_map;
#endif

    auto read_input = [&]() -> std::map<Point, char>
    {
        std::vector<std::string> garden;
        std::string line;

        while (std::getline(file, line))
        {
            if (!start.second)
            {
                start.first = line.find('S');
                if (start.first != std::string::npos)
                    start.second = garden.size();
            }
            garden.push_back(line);
        }

        size_x = line.size();
        size_y = garden.size();
        DCHECK(size_x == size_y);
        std::map<Point, char> garden_map;

        for (int64_t y = 0; y < size_y; y++)
        {
            for (int64_t x = 0; x < size_x; x++)
            {
                if (garden[y][x] == '#')
                {
                    garden_map[{x - start.first,
                                y - start.second}] = garden[y][x];
#ifdef DPRINT_ON
                    print_map[{x - start.first,
                               y - start.second}] = garden[y][x];
#endif
                }
            }
        }

        return garden_map;
    };

    const auto garden_map = read_input();
    std::map<Point, int64_t> steps_map{{{0, 0}, 0}};

    DPRINT2(size_x, size_y);
    DPRINT(start);
    DPRINT_ENDL();

    auto four_neighbors = [&](const Point &p) -> std::vector<Point>
    {
        static const std::vector<Point>
            the_four{{1, 0}, {0, 1}, {-1, 0}, {0, -1}};
        std::vector<Point> vp;
        for (const auto &ap : the_four)
        {
            const int64_t ax = p.first + ap.first;
            const int64_t ay = p.second + ap.second;
#ifdef PART_1
            if (ax + start.first >= 0 && ax + start.first < size_x &&
                ay + start.second >= 0 && ay + start.second < size_y)
#endif
            {
                vp.push_back({ax, ay});
            }
        }
        return vp;
    };

    auto find_in_garden_map = [&](Point p) -> bool
    {
        p.first += start.first;
        p.second += start.second;
        while (p.first < 0)
            p.first += size_x;
        while (p.second < 0)
            p.second += size_y;
        p.first %= size_x;
        p.second %= size_y;
        p.first -= start.first;
        p.second -= start.second;
        return garden_map.find(p) != garden_map.end();
    };

    auto do_step = [&](int64_t step) -> void
    {
        for (auto it = steps_map.begin(); it != steps_map.end();)
        {
            if (it->second <= step)
            {
                for (const auto &ap : four_neighbors(it->first))
                {
                    if (!find_in_garden_map(ap) &&
                        steps_map.find(ap) == steps_map.end())
                    {
                        steps_map[ap] = step + 1;

                        infinite_x.first =
                            std::min(infinite_x.first, ap.first);
                        infinite_x.second =
                            std::max(infinite_x.second, ap.first);
#ifdef DPRINT_ON
                        print_map[{ap.first, ap.second}] = 'O';
                    }
                    print_map[{it->first.first, it->first.second}] = '.';
#else
                    }
#endif
                }
                it = steps_map.erase(it);
            }
            else
                it++;
        }
    };

    int64_t garden_plots;
#ifdef PART_1
    for (int64_t step = 0; step < 64; step++)
    {
        do_step(step);
        garden_plots = steps_map.size();
    }

    DPRINT3(print_map, '.', 1);
    return garden_plots;
#else
    const int64_t steps = start.first + size_x * 2;

    for (int64_t step = 0; step < steps; step++)
    {
        do_step(step);
        garden_plots = steps_map.size();
    }

    const int64_t width = infinite_x.second - infinite_x.first + 1;
    const int64_t repeat = (infinite_x.second + start.first) / size_x;
    const int64_t grid_size = repeat * 2 + 1;

    std::vector<std::vector<int>> plots_grid(
        grid_size, std::vector<int>(grid_size, 0));

    for (auto it : steps_map)
    {
        auto x = it.first.first;
        auto y = it.first.second;
        x += x < 0 ? -start.first : start.first;
        y += y < 0 ? -start.second : start.second;
        x /= size_x;
        y /= size_y;
        x += repeat;
        y += repeat;
        plots_grid[y][x]++;
    }

    std::map<int64_t, int64_t> plots_histogram;
    for (int y = 0; y < grid_size; y++)
        for (int x = 0; x < grid_size; x++)
            if (plots_grid[y][x])
                plots_histogram[plots_grid[y][x]]++;

    if (width < 170)
        DPRINT3(print_map, '.', 1);

    if (grid_size < 56)
        DPRINT(plots_grid);

    for (const auto it : plots_histogram)
        DPRINT2(it.first, it.second);
    DPRINT_ENDL();
    CHECK(plots_histogram.size() == 14);

    auto garden_plots_for_steps = [&](int64_t steps_exactly) -> int64_t
    {
        const int64_t size = (steps_exactly - start.first) / size_x;
        return plots_grid[1][repeat] * size * size +
               plots_grid[0][repeat - 1] * size +
               plots_grid[0][repeat + 1] * size +
               plots_grid[repeat * 2][repeat - 1] * size +
               plots_grid[repeat * 2][repeat + 1] * size +
               plots_grid[2][repeat] * (size - 1) * (size - 1) +
               plots_grid[1][repeat - 1] * (size - 1) +
               plots_grid[1][repeat + 1] * (size - 1) +
               plots_grid[repeat * 2 - 1][repeat - 1] * (size - 1) +
               plots_grid[repeat * 2 - 1][repeat + 1] * (size - 1) +
               plots_grid[0][repeat] +
               plots_grid[repeat * 2][repeat] +
               plots_grid[repeat][0] +
               plots_grid[repeat][repeat * 2];
    };

    DPRINT2(garden_plots, garden_plots_for_steps(steps));
    DPRINT2(garden_plots_for_steps(458), garden_plots_for_steps(589));
    DPRINT_ENDL();

    return garden_plots_for_steps(26501365);
#endif
}
