// #include <debug_print.h>
#include <main.h>
#include <skip.h>

#include <algorithm>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

struct Blueprint
{
    const int id_number = 0;

    const int cost_ore_in_ore = 0;
    const int cost_clay_in_ore = 0;
    const int cost_obsidian_in_ore = 0;
    const int cost_obsidian_in_clay = 0;
    const int cost_geode_in_ore = 0;
    const int cost_geode_in_obsidian = 0;

    int number_of_geodes_opened = 0;
    std::thread thread;

    Blueprint() = default;

    Blueprint(int id,
              int ore_in_ore,
              int clay_in_ore,
              int obsidian_in_ore,
              int obsidian_in_clay,
              int geode_in_ore,
              int geode_in_obsidian)
        : id_number(id),
          cost_ore_in_ore(ore_in_ore),
          cost_clay_in_ore(clay_in_ore),
          cost_obsidian_in_ore(obsidian_in_ore),
          cost_obsidian_in_clay(obsidian_in_clay),
          cost_geode_in_ore(geode_in_ore),
          cost_geode_in_obsidian(geode_in_obsidian) {}

    void open_geodes();
};

struct Solution
{
    int minutes_left = 32;

    int ore = 0;
    int clay = 0;
    int obsidian = 0;
    int geode = 0;

    int robot_ore = 1;
    int robot_clay = 0;
    int robot_obsidian = 0;
    int robot_geode = 0;

    bool collect()
    {
        if (minutes_left > 0)
        {
            ore += robot_ore;
            clay += robot_clay;
            obsidian += robot_obsidian;
            geode += robot_geode;
            minutes_left--;
            return true;
        }
        return false;
    }

    bool operator<(const Solution &right) const
    {
        return minutes_left > right.minutes_left;
    }

#ifdef DPRINT_ON
    int id_blueprint = 0;
    friend std::ostream &operator<<(
        std::ostream &out, const Solution &solution);
#endif
};

#ifdef DPRINT_ON
static std::mutex mutex;
#endif

Blueprint read_blueprint(std::ifstream &file);

std::optional<uint64_t> Answer(std::ifstream &file)
{
    std::vector<Blueprint> blueprints;

    for (int i = 0; i < 3 && skip<std::string>(file); i++)
        blueprints.push_back(read_blueprint(file));

    for (auto &blueprint : blueprints)
    {
        std::thread thread(&Blueprint::open_geodes, &blueprint);
        blueprint.thread = std::move(thread);
    }

    uint64_t multiply_number_of_geodes = 1;
    for (auto &blueprint : blueprints)
    {
        if (blueprint.thread.joinable())
            blueprint.thread.join();

        multiply_number_of_geodes *= blueprint.number_of_geodes_opened;
    }

    return multiply_number_of_geodes;
}

void Blueprint::open_geodes()
{
    std::priority_queue<Solution> solution_queue;
    solution_queue.push({});

    while (true)
    {
        auto solution = std::move(solution_queue.top());
        solution_queue.pop();

        if (solution.minutes_left > 0)
        {
            if (solution.robot_ore < std::max({cost_clay_in_ore,
                                               cost_obsidian_in_ore,
                                               cost_geode_in_ore}))
            {
                auto copy = solution;
                copy.collect();
                solution_queue.push(std::move(copy));

                if (solution.ore >= cost_ore_in_ore)
                {
                    auto copy = solution;
                    copy.ore -= cost_ore_in_ore;
                    copy.collect();
                    copy.robot_ore++;
                    solution_queue.push(std::move(copy));
                }
            }

            if (solution.ore >= cost_geode_in_ore &&
                solution.obsidian >= cost_geode_in_obsidian)
            {
                auto copy = solution;
                copy.ore -= cost_geode_in_ore;
                copy.obsidian -= cost_geode_in_obsidian;
                copy.collect();
                copy.robot_geode++;
                solution_queue.push(std::move(copy));
                continue;
            }

            if (solution.robot_obsidian < cost_geode_in_obsidian &&
                solution.ore >= cost_obsidian_in_ore &&
                solution.clay >= cost_obsidian_in_clay)
            {
                auto copy = solution;
                copy.ore -= cost_obsidian_in_ore;
                copy.clay -= cost_obsidian_in_clay;
                copy.collect();
                copy.robot_obsidian++;
                solution_queue.push(std::move(copy));
            }

            if (solution.robot_clay < cost_obsidian_in_clay &&
                solution.ore >= cost_clay_in_ore)
            {
                auto copy = solution;
                copy.ore -= cost_clay_in_ore;
                copy.collect();
                copy.robot_clay++;
                solution_queue.push(std::move(copy));
            }
        }
        else
        {
            if (solution.geode > number_of_geodes_opened)
            {
                number_of_geodes_opened = solution.geode;
#ifdef DPRINT_ON
                solution.id_blueprint = id_number;
                std::cout << solution;
#endif
            }
        }

        if (solution_queue.empty())
            break;
    }

#ifdef DPRINT_ON
    std::lock_guard<std::mutex> guard(mutex);
    std::cout << id_number << ": " << number_of_geodes_opened << std::endl;
#endif
}

Blueprint read_blueprint(std::ifstream &file)
{
    int id, ore_in_ore, clay_in_ore, obsidian_in_ore, obsidian_in_clay,
        geode_in_ore, geode_in_obsidian;

    if (file >> id &&
        skip<std::string>(file, 5) &&
        file >> ore_in_ore &&
        skip<std::string>(file, 5) &&
        file >> clay_in_ore &&
        skip<std::string>(file, 5) &&
        file >> obsidian_in_ore &&
        skip<std::string>(file, 2) &&
        file >> obsidian_in_clay &&
        skip<std::string>(file, 5) &&
        file >> geode_in_ore &&
        skip<std::string>(file, 2) &&
        file >> geode_in_obsidian &&
        skip<std::string>(file))
    {
        return Blueprint(id, ore_in_ore, clay_in_ore, obsidian_in_ore,
                         obsidian_in_clay, geode_in_ore, geode_in_obsidian);
    }
    return {};
}

#ifdef DPRINT_ON
std::ostream &operator<<(std::ostream &out, const Solution &solution)
{
    std::lock_guard<std::mutex> guard(mutex);
    out << "[" << std::this_thread::get_id() << "."
        << solution.id_blueprint
        << "] ore, clay, obsidian, geode: ("
        << solution.ore << ", "
        << solution.clay << ", "
        << solution.obsidian << ", "
        << solution.geode << ") robots: ("
        << solution.robot_ore << ", "
        << solution.robot_clay << ", "
        << solution.robot_obsidian << ", "
        << solution.robot_geode << ") "
        << solution.minutes_left << " min. left"
        << std::endl;
    return out;
}
#endif
