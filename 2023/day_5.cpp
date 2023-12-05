#include <main.h>
#include <skip.h>

#include <queue>
#include <sstream>
#include <vector>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    using Number = unsigned int;
    std::vector<Number> seeds;
    std::string line;
    Number number;

    if (std::getline(file, line))
    {
        std::stringstream ss(line);
        skip<std::string>(ss);
        while (ss >> number)
            seeds.push_back(number);
        std::getline(file, line);
    }

    struct Map
    {
        Number destination;
        Number source;
        Number range;
    };

    using Maps = std::vector<Map>;
    std::vector<Maps> maps_list;

    while (std::getline(file, line))
    {
        Maps maps;
        while (std::getline(file, line) && !line.empty())
        {
            std::stringstream ss(line);
            Map map;
            if (ss >> map.destination >> map.source >> map.range)
                maps.push_back(std::move(map));
        }
        maps_list.push_back(std::move(maps));
    }

    std::priority_queue<Number, std::vector<Number>, std::greater<Number>>
        lowest_number;

    for (const auto &seed : seeds)
    {
        number = seed;
        for (const auto &maps : maps_list)
        {
            for (const auto &map : maps)
            {
                if (map.source <= number && number < map.source + map.range)
                {
                    number = map.destination + number - map.source;
                    break;
                }
            }
        }
        lowest_number.push(number);
    }

    return lowest_number.top();
}
