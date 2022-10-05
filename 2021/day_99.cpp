#include <iostream>
#include <list>
#include <map>
#include <string>
#include <unordered_map>

int main()
{
    const char *guid_list[] = {
        "cccccccc-cccc-cccc-cccc-cccccccccccc",
        "aaaaaaaa-aaaa-aaaa-aaaa-aaaaaaaaaaaa",
        "eeeeeeee-eeee-eeee-eeee-eeeeeeeeeeee",
        "dddddddd-dddd-dddd-dddd-dddddddddddd",
        "bbbbbbbb-bbbb-bbbb-bbbb-bbbbbbbbbbbb"};

    struct Ordered
    {
        int order = 0;
        std::string key;
    };

    std::map<std::string, int> map;
    std::unordered_map<std::string, int> unordered_map;
    std::list<std::string> order;

    for (int i = 0; i < sizeof(guid_list) / sizeof(guid_list[0]); i++)
    {
        std::string guid = guid_list[i];
        map[guid] = i;
        unordered_map[guid] = i;
        order.push_back(guid);
    }

    std::cout << "std::map - Ordered by the key." << std::endl
              << "map.begin() => ";

    for (auto it = map.begin(); it != map.end(); it++)
    {
        std::cout << it->first << " : order "
                  << it->second << std::endl
                  << "               ";
    }

    std::cout << std::endl
              << "std::unordered_map - Unordered but still organized into "
                 "buckets based on the hash of key."
              << std::endl
              << "unordered_map.begin() => ";

    for (auto it = unordered_map.begin(); it != unordered_map.end(); it++)
    {
        std::cout << it->first << " : order "
                  << it->second << std::endl
                  << "                         ";
    }

    std::cout << std::endl
              << "Iterable queue using list and unordered_map."
              << std::endl
              << "order.begin() => ";

    for (auto it = order.begin(); it != order.end(); it++)
    {
        std::cout << *it << " : order "
                  << unordered_map[*it] << std::endl
                  << "                 ";
    }

    std::cout << std::endl;

    return 0;
}
