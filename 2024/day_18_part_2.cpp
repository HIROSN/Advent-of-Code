#include <debug_print.h>
#undef DPRINT_ON
#undef DTIMER_ON
#include <main.h>
#include <skip.h>
#include <traveler.h>

#include <functional>
#include <iterator>
#include <map>
#include <queue>
#include <utility>
#include <vector>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    std::vector<aoc::Point> list_of_bytes;
    aoc::Point start, end, bytes;

    while (file >> bytes.x)
    {
        skip<char>(file);
        file >> bytes.y;

        if (bytes.x > end.x)
            end.x = bytes.x;
        if (bytes.y > end.y)
            end.y = bytes.y;

        list_of_bytes.push_back(bytes);
    }

    const Number size_x = end.x + 1;
    const Number size_y = end.y + 1;
    DPRINT2(size_x, size_y);
    DPRINT_ENDL();

    auto run = [&](int bytes_to_fall) -> Number
    {
        std::vector<std::vector<Number>> minimum_steps(
            size_y, std::vector<Number>(size_x, NUMBER_MAX));

        std::map<std::pair<Number, Number>, char> fallen;

        for (int i = 0; i < bytes_to_fall; i++)
            fallen[list_of_bytes[i].as_pair()] = '#';

        std::vector<std::string> memory_space = mpc_to_vs(fallen, '.', 1);
        DPRINT(memory_space);
        using Runner = aoc::Traveler;

        auto lowest_at = [&](const Runner &r) -> Number &
        {
            return minimum_steps[r.y][r.x];
        };

        auto corrupted = [&](const Runner &r, const aoc::Offset &o) -> bool
        {
            const Number x = r.x + o.x;
            const Number y = r.y + o.y;
            if (x >= 0 && x < size_x && y >= 0 && y < size_y)
                return memory_space[y][x] == '#';
            return true;
        };

        using RunnerQueue =
            std::priority_queue<std::pair<Number, Runner>,
                                std::vector<std::pair<Number, Runner>>,
                                std::greater<std::pair<Number, Runner>>>;

        RunnerQueue runner_queue;
        runner_queue.emplace(
            std::make_pair(0, Runner(start, end, size_x, size_y)));

        while (!runner_queue.empty())
        {
            auto runner = runner_queue.top().second;
            runner_queue.pop();

            if (runner.visited.size() >= lowest_at(runner))
                continue;

            lowest_at(runner) = runner.visited.size();
            if (runner.is_at_end())
            {
                DPRINT3(runner.get_path('.', 'O'), '.', 1);
                continue;
            }

            for (const auto &offset : runner.direction.offsets(3))
            {
                if (corrupted(runner, offset))
                    continue;

                auto next = runner;
                next.visit(offset);
                runner_queue.push({next.visited.size(), next});
            }
        }

        return minimum_steps[end.y][end.x];
    };

    int falling_start = list_of_bytes.size() < 1024 ? 12 : 1024;
    int falling_end = list_of_bytes.size();

    while (falling_start < falling_end)
    {
        int falling_half = (falling_start + falling_end) / 2;
        auto steps = run(falling_half);
        DPRINT2(falling_half, steps - 1);
        DPRINT_ENDL();

        if (falling_half == falling_start)
        {
            auto prevent_exit = list_of_bytes[falling_end - 1];
            std::cout << prevent_exit.x << ',' << prevent_exit.y << std::endl;
            break;
        }

        if (steps < NUMBER_MAX)
            falling_start = falling_half;
        else
            falling_end = falling_half;
    }

    return {};
}
