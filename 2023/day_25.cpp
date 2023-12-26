// #include <debug_print.h>
#include <main.h>

#include <algorithm>
#include <functional>
#include <map>
#include <queue>
#include <sstream>
#include <utility>
#include <vector>

constexpr int max_number_of_components = 1550;
int index_components[26][26][26] = {};
std::map<int, bool> connection_map[max_number_of_components];
std::vector<std::pair<std::string, std::string>> connections;
std::map<std::string, std::vector<std::string>> component_connections;
std::vector<std::string> components;

int index(const std::string &component)
{
    return index_components[component[0] - 'a']
                           [component[1] - 'a']
                           [component[2] - 'a'];
}

bool &is_connected(const std::string &left, const std::string &right)
{
    if (left < right)
        return connection_map[index(left)][index(right)];
    else
        return connection_map[index(right)][index(left)];
}

std::map<int, std::vector<std::string>> get_groups()
{
    std::map<int, std::vector<std::string>> groups;
    std::map<std::string, bool> visited;
    int index_group = 0;

    for (const auto &start : components)
    {
        if (visited.find(start) != visited.end())
            continue;

        std::queue<std::string> queue;
        queue.push(start);

        while (!queue.empty())
        {
            const auto component = queue.front();
            queue.pop();

            if (visited.find(component) != visited.end())
                continue;

            groups[index_group].push_back(component);
            visited[component] = true;

            for (const auto &connection : component_connections[component])
                if (is_connected(component, connection))
                    queue.push(connection);
        }

        index_group++;
    }

    return groups;
};

int get_shortest_path(std::string component, const std::string &end)
{
    const int max_length = components.size();
    std::map<std::string, int> lengths;
    std::map<std::string, bool> entered;
    std::map<std::string, bool> queued;

    for (const auto &component : components)
        lengths[component] = max_length;

    std::priority_queue<std::pair<int, std::string>,
                        std::vector<std::pair<int, std::string>>,
                        std::greater<std::pair<int, std::string>>>
        queue;

    lengths[component] = 0;
    entered[component] = true;
    DPRINTX(component << " " << end << " ");

    while (true)
    {
        for (const auto &connection : component_connections[component])
        {
            if (is_connected(component, connection))
            {
                if (!entered[connection])
                {
                    if (lengths[connection] > lengths[component] + 1)
                        lengths[connection] = lengths[component] + 1;

                    if (queued.find(connection) == queued.end())
                    {
                        queue.push({lengths[connection], connection});
                        queued[connection] = true;
                    }
                }
            }
        }

        if (!queue.empty())
        {
            component = queue.top().second;
            queue.pop();
            queued.erase(component);
            entered[component] = true;
            continue;
        }
        break;
    }

    DPRINTX_ENDL(lengths[end]);
    return lengths[end];
}

std::vector<std::pair<std::string, std::string>> get_sorted_connections()
{
    std::map<int, std::vector<std::pair<std::string, std::string>>>
        shortest_path_connections;

    for (const auto &connection : connections)
    {
        is_connected(connection.first, connection.second) = false;
        int path = get_shortest_path(connection.first, connection.second);
        shortest_path_connections[path].push_back({connection.first,
                                                   connection.second});
        is_connected(connection.first, connection.second) = true;
    }

    std::vector<std::pair<std::string, std::string>> sorted_connections;

    for (auto it = shortest_path_connections.rbegin();
         it != shortest_path_connections.rend();
         it++)
    {
        sorted_connections.insert(sorted_connections.end(),
                                  it->second.begin(), it->second.end());
    }

    return sorted_connections;
}

std::optional<uint64_t> Answer(std::ifstream &file)
{
    std::string line;

    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string component, other;
        ss >> component;
        component = component.substr(0, component.size() - 1);
        while (ss >> other)
        {
            component_connections[component].push_back(other);
            component_connections[other].push_back(component);
            std::pair<std::string, std::string> connection;
            if (component < other)
                connection = {component, other};
            else
                connection = {other, component};
            CHECK(std::find(connections.begin(), connections.end(),
                            connection) == connections.end());
            connections.push_back(connection);
        }
    }

    for (auto it : component_connections)
    {
        static int index_component = 0;
        CHECK(index_component < max_number_of_components);
        index_components[it.first[0] - 'a']
                        [it.first[1] - 'a']
                        [it.first[2] - 'a'] = index_component++;
        components.push_back(it.first);
        DPRINTX(it.first << " (" << it.second.size() << ") : ");
        for (const auto &connection : it.second)
            DPRINTX(connection << " ");
        DPRINT_ENDL();
    }

    for (auto it : connections)
        connection_map[index(it.first)][index(it.second)] = true;

    DPRINT2(components.size(), connections.size());
    DPRINT_ENDL();

    auto sorted_connections = get_sorted_connections();

    DPRINT2(connections.size(), sorted_connections.size());
    DPRINT_ENDL();

    for (auto it1 = sorted_connections.begin();
         it1 != sorted_connections.end();
         it1++)
    {
        is_connected(it1->first, it1->second) = false;
        for (auto it2 = it1 + 1;
             it2 != sorted_connections.end();
             it2++)
        {
            is_connected(it2->first, it2->second) = false;
            for (auto it3 = it2 + 1;
                 it3 != sorted_connections.end();
                 it3++)
            {
                is_connected(it3->first, it3->second) = false;

                auto groups = get_groups();
                if (groups.size() == 2)
                    return groups[0].size() * groups[1].size();

                is_connected(it3->first, it3->second) = true;
            }
            is_connected(it2->first, it2->second) = true;
        }
        is_connected(it1->first, it1->second) = true;
    }

    return {};
}
