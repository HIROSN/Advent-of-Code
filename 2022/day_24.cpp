#include <debug_print.h>
#undef DPRINT_ON
#include <main.h>

#include <climits>
#include <cmath>
#include <list>
#include <map>
#include <queue>
#include <utility>
#include <vector>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    using Position = std::pair<int, int>;
    using Valley = std::map<Position, std::vector<char>>;

    Valley valley;
    Position non_wall_top, non_wall_bottom;
    int valley_size_x, valley_size_y;

    const char wall_mark = '#';
    const char clear_ground = '.';

    for (int y = 0; true; y++)
    {
        std::string line;
        if (!std::getline(file, line))
        {
            valley_size_y = y - 2;
            break;
        }

        for (int x = 0; x < line.size(); x++)
        {
            if (line[x] != clear_ground)
                valley[{x, y}].push_back(line[x]);
            else
                non_wall_bottom = {x, y};
        }

        if (y == 0)
        {
            valley_size_x = line.size() - 2;
            non_wall_top = non_wall_bottom;
        }
    }

    using Direction = Position;
    std::map<char, Direction> directions = {
        {'v', {0, +1}},
        {'>', {+1, 0}},
        {'<', {-1, 0}},
        {'^', {0, -1}},
    };

    auto next_blizzard_position =
        [&](const Position &p,
            const Direction &d) -> Position
    {
        int x = p.first + d.first - 1;
        int y = p.second + d.second - 1;
        x += (x < 0 ? valley_size_x : 0);
        y += (y < 0 ? valley_size_y : 0);
        x = (x % valley_size_x) + 1;
        y = (y % valley_size_y) + 1;
        return {x, y};
    };

    auto move_blizzards = [&](Valley &valley) -> void
    {
        Valley moved;
        for (const auto &map : valley)
        {
            const auto &position = map.first;
            const auto &blizzards = map.second;

            for (const auto &blizzard : blizzards)
            {
                if (directions.find(blizzard) != directions.end())
                    moved[next_blizzard_position(
                              position,
                              directions[blizzard])]
                        .push_back(blizzard);
                else
                    moved[position].push_back(blizzard);
            }
        }
        valley = std::move(moved);
    };

    auto is_valid_direction =
        [&](const Position &p,
            const Direction &d,
            Position &next) -> bool
    {
        int x = p.first + d.first;
        int y = p.second + d.second;
        next = {x, y};
        return x >= 0 && x < (valley_size_x + 2) &&
               y >= 0 && y < (valley_size_y + 2);
    };

    auto draw_valley = [&](const Valley &valley) -> std::vector<std::string>
    {
        std::map<Position, char> mpc;
        for (const auto &map : valley)
        {
            if (map.second.size() == 1)
                mpc[map.first] = map.second[0];
            else if (map.second.size() <= 9)
                mpc[map.first] = '0' + map.second.size();
        }
        return mpc_to_vs(mpc, clear_ground, 1);
    };

    std::vector<std::vector<std::string>> drawings;
    std::map<int, Valley> valleys;
    drawings.push_back(draw_valley(valley));
    valleys[0] = std::move(valley);

    std::vector<std::vector<std::vector<bool>>> visited_at_this_minute;
    const std::vector<bool> row_not_visited(
        drawings[0][0].size(), false);
    const std::vector<std::vector<bool>> all_not_visited(
        drawings[0].size(), row_not_visited);
    visited_at_this_minute.push_back(all_not_visited);

    struct Solution
    {
        Position current;
        Position goal;
        int minutes = 0;

        Solution(const Position &start, const Position &end)
            : current(start), goal(end) {}

        int distance_to_goal() const
        {
            return std::abs(goal.first - current.first) +
                   std::abs(goal.second - current.second);
        }

        bool operator<(const Solution &right) const
        {
            return distance_to_goal() > right.distance_to_goal();
        }

#ifdef DPRINT_ON
        std::string direction;
#endif
    };

    std::priority_queue<Solution> solution_queue;
    solution_queue.push(Solution(non_wall_top, non_wall_bottom));
    int fewest_number_of_minutes = INT_MAX;

    while (true)
    {
        auto solution = solution_queue.top();
        solution_queue.pop();

        const auto &current = solution.current;
        const auto &x = current.first;
        const auto &y = current.second;

        bool reached_goal = current == non_wall_bottom;
        bool abort = visited_at_this_minute[solution.minutes][y][x] ||
                     solution.minutes + solution.distance_to_goal() >=
                         fewest_number_of_minutes;
        bool fewest = reached_goal && solution.minutes <
                                          fewest_number_of_minutes;
        visited_at_this_minute[solution.minutes][y][x] = true;

#ifdef DPRINT_ON
        if (fewest)
        {
            std::cout << "Minute " << solution.minutes << " ";
            if (solution.direction.size())
                std::cout << solution.direction << " ";
            std::cout << (reached_goal ? (fewest ? "fewest goal " : "goal ")
                                       : (abort ? "abort " : ""))
                      << "[queue size " << solution_queue.size() + 1 << "] "
                      << std::endl;
            auto drawing_copy = drawings[solution.minutes];
            drawing_copy[y][x] = 'E';
            DPRINT(drawing_copy);
        }
        static int operations_since_last_fewest = 0;
        if (fewest)
            operations_since_last_fewest = 0;
        else
            operations_since_last_fewest++;
#endif

        if (reached_goal)
        {
            if (fewest)
                fewest_number_of_minutes = solution.minutes;
        }
        else if (!abort)
        {
            solution.minutes++;

            if (drawings.size() == solution.minutes)
            {
                auto new_valley = valleys[solution.minutes - 1];
                move_blizzards(new_valley);
                drawings.push_back(draw_valley(new_valley));
                valleys[solution.minutes] = std::move(new_valley);
                visited_at_this_minute.push_back(all_not_visited);
            }

            const auto &drawing = drawings[solution.minutes];

            for (const auto &map : directions)
            {
                const auto &direction = map.second;
                Position new_position;

                if (is_valid_direction(current, direction, new_position))
                {
                    const auto &new_x = new_position.first;
                    const auto &new_y = new_position.second;

                    if (drawing[new_y][new_x] == clear_ground)
                    {
                        auto copy = solution;
                        copy.current = new_position;
#ifdef DPRINT_ON
                        copy.direction = map.first;
#endif
                        solution_queue.push(std::move(copy));
                    }
                }
            }

            if (drawing[y][x] == clear_ground)
            {
#ifdef DPRINT_ON
                solution.direction = "wait";
#endif
                solution_queue.push(std::move(solution));
            }
        }

        if (solution_queue.empty())
        {
#ifdef DPRINT_ON
            std::cout << operations_since_last_fewest
                      << " operations since last fewest" << std::endl;
#endif
            break;
        }
    }

    return fewest_number_of_minutes;
}
