// #include <debug_print.h>
#include <main.h>
#include <skip.h>

#include <algorithm>
#include <list>
#include <map>
#include <queue>
#include <sstream>
#include <utility>
#include <vector>

// #define PART_1

using Computer = std::pair<char, char>;
using ComputerList = std::vector<Computer>;

std::ostream &operator<<(std::ostream &out, const Computer &c)
{
    out << c.first << c.second;
    return out;
}

std::ostream &operator<<(std::ostream &out, const ComputerList &cl)
{
    for (auto it = cl.begin(); it != cl.end(); it++)
    {
        if (it != cl.begin())
            out << ',';
        out << *it;
    }
    return out;
}

std::optional<uint64_t> Answer(std::ifstream &file)
{
    std::vector<ComputerList> connection_list;
    std::map<Computer, ComputerList> network_map;
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

    auto print_lists =
        [](const std::vector<ComputerList> &lists) -> std::string
    {
        std::stringstream ss;
        ss << lists.size() << std::endl;
        for (const auto &cl : lists)
            ss << cl << std::endl;
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

    auto is_connected = [&](const Computer &c1,
                            const Computer &c2) -> bool
    {
        if (c1 == c2)
            return false;
        for (auto c : network_map[c1])
            if (c == c2)
                return true;
        return false;
    };

    auto sort_computers = [](ComputerList list) -> ComputerList
    {
        std::priority_queue<Computer> queue;
        for (const auto &c : list)
            queue.push(c);
        list.clear();
        while (!queue.empty())
        {
            list.insert(list.begin(), queue.top());
            queue.pop();
        }
        return list;
    };

    DPRINTX(print_lists(connection_list));
    DPRINTX(print_network_map());

    auto is_inter_connected = [&](const ComputerList &list) -> bool
    {
        for (int l = 0; l < list.size() - 1; l++)
            for (int r = l + 1; r < list.size(); r++)
                if (!is_connected(list[l], list[r]))
                    return false;
        return true;
    };

    auto find_inter_connected = [&](const std::vector<ComputerList> &lists)
        -> std::vector<ComputerList>
    {
        std::vector<ComputerList> inter_connected;
        for (auto it : network_map)
        {
            const auto &c = it.first;
            for (auto cl : lists)
            {
                cl.push_back(c);
                if (is_inter_connected(cl))
                {
                    cl = sort_computers(cl);
                    if (std::find(inter_connected.begin(),
                                  inter_connected.end(),
                                  cl) == inter_connected.end())
                    {
                        inter_connected.push_back(cl);
                    }
                }
            }
        }

        return inter_connected;
    };

#ifdef PART_1
    auto inter_connected = find_inter_connected(connection_list);
    DPRINTX_ENDL(inter_connected.size());
    int starts_with_t = 0;

    for (const auto &tc : inter_connected)
    {
        for (const auto &c : tc)
        {
            if (c.first == 't')
            {
                starts_with_t++;
                DPRINTX_ENDL(tc);
                break;
            }
        }
    }

    return starts_with_t;
#else
    std::vector<ComputerList> inter_connected;
    int largest = 0;

    std::function<void(const ComputerList &)> find_largest =
        [&](const ComputerList &list) -> void
    {
        for (auto it : network_map)
        {
            const auto &c = it.first;
            if (std::find(list.begin(), list.end(), c) != list.end())
                continue;

            auto list_copy = list;
            list_copy.push_back(c);
            if (!is_inter_connected(list_copy))
                continue;

            int size = list_copy.size();
            if (size > largest)
                largest = size;

            if (size != largest)
                continue;

            list_copy = sort_computers(list_copy);
            if (std::find(inter_connected.begin(), inter_connected.end(),
                          list_copy) != inter_connected.end())
                continue;

            inter_connected.push_back(list_copy);
            DPRINTX_ENDL(list_copy);
            find_largest(list_copy);
        }
    };

    for (auto it : network_map)
    {
        ComputerList list{it.first};
        for (const auto &c : it.second)
        {
            list.push_back(c);
            find_largest(list);
        }
    }

    for (const auto &cl : inter_connected)
        if (cl.size() == largest)
            std::cout << cl << std::endl;

    return {};
#endif
}
