// #include <debug_print.h>
#include <main.h>
#include <skip.h>

#include <algorithm>
#include <climits>
#include <functional>
#include <list>
#include <queue>
#include <sstream>
#include <unordered_map>
#include <utility>
#include <vector>

std::unordered_map<std::string, int> flow_rates;
int most_pressure_released = 0;

struct Tunnel
{
    std::string lead_to_valve;
    int minutes;
};

class Compare
{
public:
    bool operator()(Tunnel left, Tunnel right)
    {
        return left.minutes > right.minutes;
    }
};

using ActiveValveQueue =
    std::priority_queue<Tunnel, std::vector<Tunnel>, Compare>;

std::unordered_map<std::string, ActiveValveQueue> all_valves;
std::unordered_map<std::string, ActiveValveQueue> active_valves;

void read_input(std::ifstream &file);
std::vector<std::string> active_valve_names();
void find_shortest_path_to_active_valves(std::string start);

#ifdef DPRINT_ON
void print_valves(
    const std::unordered_map<std::string, ActiveValveQueue> &valves);
#endif

struct Solution
{
    std::list<std::string> open_valves;
    int released_pressure = 0;
    int minutes = 30;

    bool open(const std::string &valve, int moved)
    {
        if (minutes >= moved + 1 &&
            (open_valves.empty() || std::find(open_valves.begin(),
                                              open_valves.end(),
                                              valve) == open_valves.end()))
        {
            minutes -= moved + 1;
            open_valves.push_back(valve);
            if (minutes > 0)
                released_pressure += flow_rates[valve] * minutes;
            return true;
        }
        return false;
    }

    bool operator<(const Solution &right) const
    {
        return released_pressure < right.released_pressure;
    }

#ifdef DPRINT_ON
    friend std::ostream &operator<<(
        std::ostream &out, const Solution &solution);
#endif
};

std::optional<uint64_t> Answer(std::ifstream &file)
{
    read_input(file);

    const std::string first_valve = "AA";
    find_shortest_path_to_active_valves(first_valve);
    for (const auto &active_valve : active_valve_names())
        find_shortest_path_to_active_valves(active_valve);

#ifdef DPRINT_ON
    DPRINT_ENDL();
    print_valves(active_valves);
    DPRINT_ENDL();
#endif

    std::priority_queue<Solution> solution_queue;
    solution_queue.push({});

    while (true)
    {
        auto solution = solution_queue.top();
        solution_queue.pop();

        std::string valve;
        if (solution.open_valves.empty())
            valve = first_valve;
        else
            valve = solution.open_valves.back();

        auto adjacent_valves = active_valves[valve];
        while (!adjacent_valves.empty())
        {
            const auto &tunnel = adjacent_valves.top();
            const auto &adjacent = tunnel.lead_to_valve;
            auto copy = solution;
            if (copy.open(adjacent, tunnel.minutes))
            {
#ifdef DPRINT_ON
                std::cout << copy;
#endif
                if (copy.released_pressure > most_pressure_released)
                    most_pressure_released = copy.released_pressure;
                solution_queue.push(std::move(copy));
            }
            adjacent_valves.pop();
        }

        if (solution_queue.empty())
            break;
    }

    if (most_pressure_released)
        return most_pressure_released;

    return {};
}

void read_input(std::ifstream &file)
{
    while (skip<std::string>(file))
    {
        std::string valve, lead_to(2, ' '), line;
        int flow_rate;

        file >> valve;
        skip(file, '=');
        file >> flow_rate;
        flow_rates[valve] = flow_rate;

        skip<std::string>(file, 5);
        std::getline(file, line);
        std::stringstream ss(line);
        while (ss >> lead_to[0] >> lead_to[1])
        {
            all_valves[valve].push(Tunnel{lead_to, 1});
            skip<char>(ss);
        }
    }

#ifdef DPRINT_ON
    auto active_valves = active_valve_names();
    std::cout << active_valves.size() << " active valves: ";
    for (const auto &valve : active_valves)
        std::cout << valve << " ";
    std::cout << std::endl;
    print_valves(all_valves);
#endif
}

std::vector<std::string> active_valve_names()
{
    static std::vector<std::string> names;
    if (names.empty())
        for (const auto &flow_rate : flow_rates)
            if (flow_rate.second > 0)
                names.push_back(flow_rate.first);
    return names;
}

void find_shortest_path_to_active_valves(std::string start)
{
    std::unordered_map<std::string, int> minutes;
    std::unordered_map<std::string, bool> visited;

    for (const auto &valve : all_valves)
    {
        minutes[valve.first] = INT_MAX;
        visited[valve.first] = false;
    }

    using AdjacentValveQueue =
        std::priority_queue<std::pair<int, std::string>,
                            std::vector<std::pair<int, std::string>>,
                            std::greater<std::pair<int, std::string>>>;
    AdjacentValveQueue visited_valves;
    std::unordered_map<std::string, bool> queued;
    auto valve = start;
    minutes[start] = 0;
    visited[start] = true;

    while (true)
    {
        auto adjacent_valves = all_valves[valve];
        while (!adjacent_valves.empty())
        {
            const auto &tunnel = adjacent_valves.top();
            const auto &adjacent = tunnel.lead_to_valve;

            if (!visited[adjacent])
            {
                if (minutes[adjacent] > minutes[valve] + tunnel.minutes)
                {
                    minutes[adjacent] = minutes[valve] + tunnel.minutes;
                }

                if (queued.find(adjacent) == queued.end())
                {
                    visited_valves.push({minutes[adjacent], adjacent});
                    queued[adjacent] = true;
                }
            }
            adjacent_valves.pop();
        }

        if (!visited_valves.empty())
        {
            visited[valve] = true;
            valve = visited_valves.top().second;
            visited_valves.pop();
            queued.erase(valve);
        }
        else
        {
            break;
        }
    }

    for (const auto &valve : active_valve_names())
        if (valve != start)
            active_valves[start].push(Tunnel{valve, minutes[valve]});
}

#ifdef DPRINT_ON
void print_valves(
    const std::unordered_map<std::string, ActiveValveQueue> &valves)
{
    for (const auto &valve : valves)
    {
        std::cout << "Valve " << valve.first
                  << "(" << flow_rates[valve.first] << ") : ";
        auto valve_queue = valve.second;
        while (!valve_queue.empty())
        {
            const auto &tunnel = valve_queue.top();
            std::cout << tunnel.lead_to_valve
                      << "(" << tunnel.minutes << ") ";
            valve_queue.pop();
        }
        std::cout << std::endl;
    }
}

std::ostream &operator<<(std::ostream &out, const Solution &solution)
{
    if (solution.released_pressure > most_pressure_released)
    {
        out << solution.released_pressure << ": ";
        for (const auto &valve : solution.open_valves)
            out << valve << " ";
        out << std::endl;
    }
    return out;
}
#endif
