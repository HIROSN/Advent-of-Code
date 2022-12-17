// #include <debug_print.h>
#include <main.h>
#include <skip.h>

#include <algorithm>
#include <climits>
#include <functional>
#include <queue>
#include <sstream>
#include <unordered_map>
#include <utility>
#include <vector>

constexpr int max_number_of_valves = 58;
constexpr int max_number_of_valve_chars = 'Z' - 'A' + 1;
constexpr int max_number_of_travelers = 2;
constexpr int max_number_of_minutes = 26;

struct Valve
{
    char left = 0;
    char right = 0;
    int minutes = 0;

    Valve() = default;

    Valve(int index_left, int index_right)
    {
        left = static_cast<char>(index_left + 'A');
        right = static_cast<char>(index_right + 'A');
    }

    Valve(const std::string &name)
    {
        left = name[0];
        right = name[1];
    }

    Valve(const std::string &name, int min)
    {
        left = name[0];
        right = name[1];
        minutes = min;
    }

    bool empty() const
    {
        return !left || !right;
    }

    int index_left() const
    {
        return left - 'A';
    }

    int index_right() const
    {
        return right - 'A';
    }

    std::string str() const
    {
        std::stringstream ss;
        ss << left;
        ss << right;
        return ss.str();
    };

    bool operator==(const Valve &r) const
    {
        return left == r.left && right == r.right;
    }
};

class CompareValve
{
public:
    bool operator()(Valve left, Valve right)
    {
        return left.minutes > right.minutes;
    }
};

using ValveQueue =
    std::priority_queue<Valve, std::vector<Valve>, CompareValve>;

std::unordered_map<std::string, ValveQueue> all_valves;

Valve active_valves[max_number_of_valve_chars]
                   [max_number_of_valve_chars]
                   [max_number_of_valves] = {};

int flow_rates[max_number_of_valve_chars][max_number_of_valve_chars] = {};
int most_pressure_released = 0;

struct Traveler
{
    int minutes_left = max_number_of_minutes;
    Valve last_open;
};

struct Solution
{
    int released_pressure = 0;
    Valve open_valves[max_number_of_valves] = {};
    Traveler travelers[max_number_of_travelers] = {};

    Solution() = default;

    Solution(const std::string &first_valve)
    {
        for (int i = 0; i < max_number_of_travelers; i++)
            travelers[i].last_open = Valve(first_valve);
    }

    bool open(const Valve &valve, int traveler)
    {
        if (is_closed(valve) &&
            travelers[traveler].minutes_left >= valve.minutes + 1)
        {
            travelers[traveler].minutes_left -= valve.minutes + 1;
            released_pressure +=
                flow_rates[valve.index_left()][valve.index_right()] *
                travelers[traveler].minutes_left;
            for (int i = 0; i < max_number_of_valves; i++)
            {
                if (open_valves[i].empty())
                {
                    open_valves[i] = valve;
                    travelers[traveler].last_open = valve;
                    return true;
                }
            }
        }
        return false;
    }

    bool is_closed(const Valve &valve)
    {
        for (int i = 0; i < max_number_of_valves; i++)
        {
            if (open_valves[i].empty())
                break;
            if (valve == open_valves[i])
                return false;
        }
        return true;
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

void read_input(std::ifstream &file);
std::vector<std::string> active_valve_names();
void find_shortest_path_to_active_valves(std::string start);

#ifdef DPRINT_ON
void print_valves(
    const std::unordered_map<std::string, ValveQueue> &valves);
void print_active_valves(const std::string &first_valve);
#endif

std::optional<uint64_t> Answer(std::ifstream &file)
{
    read_input(file);

    const std::string first_valve = "AA";
    find_shortest_path_to_active_valves(first_valve);
    for (const auto &active_valve : active_valve_names())
        find_shortest_path_to_active_valves(active_valve);

#ifdef DPRINT_ON
    DPRINT_ENDL();
    print_active_valves(first_valve);
    DPRINT_ENDL();
#endif

    std::priority_queue<Solution> solution_queue;
    solution_queue.push(Solution(first_valve));

    while (true)
    {
        auto solution = solution_queue.top();
        solution_queue.pop();

        int index_valve[max_number_of_travelers] = {};
        Valve valve[max_number_of_travelers][max_number_of_valves] = {};
        for (int t = 0; t < max_number_of_travelers; t++)
        {
            const auto &last_open = solution.travelers[t].last_open;
            for (int v = 0; v < max_number_of_valves; v++)
            {
                valve[t][v] = active_valves[last_open.index_left()]
                                           [last_open.index_right()]
                                           [v];
                if (valve[t][v].empty())
                    break;
            }
        }

        while (true)
        {
            auto copy = solution;
            int index_traveler = -1;
            int most_minutes_left = 0;
            for (int t = 0; t < max_number_of_travelers; t++)
            {
                if (valve[t][index_valve[t]].empty())
                    continue;
                int minutes_left = copy.travelers[t].minutes_left -
                                   valve[t][index_valve[t]].minutes;
                if (minutes_left > 0 && minutes_left > most_minutes_left)
                {
                    index_traveler = t;
                    most_minutes_left = minutes_left;
                }
            }

            if (index_traveler < 0)
                break;

            const auto &valve_to_open =
                valve[index_traveler][index_valve[index_traveler]];

            if (copy.open(valve_to_open, index_traveler))
            {
#ifdef DPRINT_ON
                std::cout << copy;
#endif
                if (copy.released_pressure > most_pressure_released)
                    most_pressure_released = copy.released_pressure;

                solution_queue.push(std::move(copy));
            }

            index_valve[index_traveler]++;
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
        Valve v(valve);
        flow_rates[v.index_left()][v.index_right()] = flow_rate;

        skip<std::string>(file, 5);
        std::getline(file, line);
        std::stringstream ss(line);
        while (ss >> lead_to[0] >> lead_to[1])
        {
            all_valves[valve].push(Valve(lead_to, 1));
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
        for (int il = 0; il < max_number_of_valve_chars; il++)
            for (int ir = 0; ir < max_number_of_valve_chars; ir++)
                if (flow_rates[il][ir])
                    names.push_back(Valve(il, ir).str());
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
            const auto &av = adjacent_valves.top();
            const auto &adjacent = av.str();

            if (!visited[adjacent])
            {
                if (minutes[adjacent] > minutes[valve] + av.minutes)
                {
                    minutes[adjacent] = minutes[valve] + av.minutes;
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

    ValveQueue active_valve_queue;
    for (const auto &valve : active_valve_names())
    {
        if (valve == start)
            continue;
        Valve v(valve, minutes[valve]);
        active_valve_queue.push(std::move(v));
    }

    Valve vs(start);
    int i = 0;
    while (!active_valve_queue.empty())
    {
        active_valves[vs.index_left()][vs.index_right()][i++] =
            active_valve_queue.top();
        active_valve_queue.pop();
    }
}

#ifdef DPRINT_ON
void print_valves(
    const std::unordered_map<std::string, ValveQueue> &valves)
{
    for (const auto &valve : valves)
    {
        Valve v(valve.first);
        std::cout << "Valve " << v.str()
                  << "(" << flow_rates[v.index_left()][v.index_right()]
                  << ") : ";
        auto valve_queue = valve.second;
        while (!valve_queue.empty())
        {
            const auto &av = valve_queue.top();
            std::cout << av.str()
                      << "(" << av.minutes << ") ";
            valve_queue.pop();
        }
        std::cout << std::endl;
    }
}

void print_active_valves(const std::string &first_valve)
{
    auto valve_names = active_valve_names();
    valve_names.insert(valve_names.begin(), first_valve);
    for (const auto &valve : valve_names)
    {
        Valve v(valve);
        std::cout << "Valve " << v.str()
                  << "(" << flow_rates[v.index_left()][v.index_right()]
                  << ") : ";
        for (int i = 0; i < max_number_of_valves; i++)
        {
            const auto &lead_to_valve =
                active_valves[v.index_left()][v.index_right()][i];
            if (lead_to_valve.empty())
                break;
            std::cout << lead_to_valve.str()
                      << "(" << lead_to_valve.minutes << ") ";
        }
        std::cout << std::endl;
    }
}

std::ostream &operator<<(std::ostream &out, const Solution &solution)
{
    if (solution.released_pressure > most_pressure_released)
    {
        out << solution.released_pressure << ": ";
        for (int i = 0; i < max_number_of_valves; i++)
        {
            if (solution.open_valves[i].empty())
                break;
            out << solution.open_valves[i].str() << " ";
        }
        out << std::endl;
    }
    return out;
}
#endif
