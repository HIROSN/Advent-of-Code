// #include <debug_print.h>
#include <main.h>
#include <skip.h>

#include <algorithm>
#include <map>
#include <queue>
#include <sstream>
#include <utility>
#include <vector>

using Computer = std::pair<char, char>;
using Connection = std::pair<Computer, Computer>;
using ThreeComputers = std::pair<Computer, std::pair<Computer, Computer>>;

std::ostream &operator<<(std::ostream &out, const Computer &c)
{
#ifdef DPRINT_ON
    out << c.first << c.second;
#endif
    return out;
}

std::ostream &operator<<(std::ostream &out, const ThreeComputers &tc)
{
#ifdef DPRINT_ON
    out << tc.first << ',' << tc.second.first << ',' << tc.second.second;
#endif
    return out;
}

std::optional<uint64_t> Answer(std::ifstream &file)
{
    std::vector<Connection> connection_list;
    std::map<Computer, std::vector<Computer>> network_map;
    std::string line;

    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        Computer left, right;
        ss >> left.first >> left.second;
        skip<char>(ss);
        ss >> right.first >> right.second;
        if (left < right)
            connection_list.push_back({left, right});
        else
            connection_list.push_back({right, left});
        network_map[left].push_back(right);
        network_map[right].push_back(left);
    }

    auto print_connection_list = [&]() -> std::string
    {
        std::stringstream ss;
        ss << connection_list.size() << std::endl;
        for (auto c : connection_list)
            ss << c.first << ',' << c.second << std::endl;
        return ss.str();
    };

    auto print_network_map = [&]() -> std::string
    {
        std::stringstream ss;
        ss << network_map.size() << std::endl;
        for (auto it : network_map)
        {
            ss << it.first;
            for (auto c : it.second)
                ss << ',' << c;
            ss << std::endl;
        }
        return ss.str();
    };

    DPRINTX(print_connection_list());
    DPRINTX(print_network_map());

    auto is_connected = [&](const Computer &c1,
                            const Computer &c2) -> bool
    {
        for (auto c : network_map[c1])
            if (c == c2)
                return true;
        return false;
    };

    std::vector<ThreeComputers> inter_connected;

    auto make_three_computers =
        [](Computer c1, Computer c2, Computer c3) -> ThreeComputers
    {
        std::priority_queue<Computer> queue;
        queue.push(c1);
        queue.push(c2);
        queue.push(c3);
        c3 = queue.top();
        queue.pop();
        c2 = queue.top();
        queue.pop();
        c1 = queue.top();
        return {c1, {c2, c3}};
    };

    for (const auto &cn : connection_list)
    {
        for (const auto &c3 : network_map[cn.first])
        {
            if (is_connected(c3, cn.second))
            {
                auto three = make_three_computers(cn.first, cn.second, c3);

                if (std::find(inter_connected.begin(), inter_connected.end(),
                              three) == inter_connected.end())
                {
                    inter_connected.push_back(three);
                }
            }
        }
    }

    auto print_inter_connected = [&]() -> std::string
    {
        std::stringstream ss;
        ss << inter_connected.size() << std::endl;
        for (const auto &tc : inter_connected)
            ss << tc << std::endl;
        return ss.str();
    };

    DPRINTX(print_inter_connected());
    int starts_with_t = 0;

    for (const auto &tc : inter_connected)
    {
        if (tc.first.first == 't' ||
            tc.second.first.first == 't' ||
            tc.second.second.first == 't')
        {
            starts_with_t++;
        }
    }

    return starts_with_t;
}
