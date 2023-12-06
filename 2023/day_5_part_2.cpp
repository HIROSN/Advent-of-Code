#include <main.h>
#include <skip.h>

#include <algorithm>
#include <sstream>
#include <vector>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    struct Map;
    struct Range;

    using Number = uint64_t;
    using Maps = std::vector<Map>;
    using MapsList = std::vector<Maps>;
    using Ranges = std::vector<Range>;

    struct Range
    {
        Number start;
        Number end;

        bool operator&&(const Range &other) const
        {
            return !(other.end < start || end < other.start);
        }

        Range operator&(const Range &other) const
        {
            return {std::max({start, other.start}),
                    std::min({end, other.end})};
        }
    };

    struct Map
    {
        Number destination;
        Number source;
        Number length;

        Map() = default;

        Map(Number start, Number end)
        {
            destination = start;
            source = start;
            length = end - start + 1;
        }

        Number end_of(Number start) const
        {
            return start + length - 1;
        }

        bool operator<(const Map &right) const
        {
            return destination < right.destination;
        }

        Range destination_range() const
        {
            return {destination, end_of(destination)};
        }

        Range to_source_range(const Range &range) const
        {
            const auto diff = source - destination;
            return {range.start + diff, range.end + diff};
        }
    };

    Number max_number = 0;
    MapsList maps_list;
    std::string line;

    while (std::getline(file, line))
    {
        Maps maps;
        Map map;
        if (maps_list.empty())
        {
            std::stringstream ss(line);
            skip<std::string>(ss);
            while (ss >> map.destination >> map.length)
            {
                max_number = std::max({max_number,
                                       map.end_of(map.destination)});
                map.source = map.destination;
                maps.push_back(map);
            }
            std::getline(file, line);
        }
        else
        {
            while (std::getline(file, line) && !line.empty())
            {
                std::stringstream ss(line);
                if (ss >> map.destination >> map.source >> map.length)
                {
                    max_number = std::max({max_number,
                                           map.end_of(map.destination),
                                           map.end_of(map.source)});
                    maps.push_back(map);
                }
            }
        }
        std::sort(maps.begin(), maps.end());
        maps_list.push_back(std::move(maps));
    }

    for (int i = 1; i < maps_list.size(); i++)
    {
        auto &maps = maps_list[i];
        const auto &last = maps[maps.size() - 1];
        if (last.end_of(last.destination) < max_number)
        {
            maps.push_back(
                Map(last.end_of(last.destination) + 1, max_number));
        }

        const auto &first = maps[0];
        if (first.destination > 0)
        {
            maps.insert(maps.begin(), Map(0, first.destination - 1));
        }
    }

    struct Seeds
    {
        bool find_range(const Range &range, const MapsList &maps_list,
                        int index, Range &found)
        {
            if (index < 0)
            {
                found = range;
                return true;
            }

            for (auto &source_range : to_source_ranges(range,
                                                       maps_list[index]))
            {
                if (find_range(source_range, maps_list, index - 1, found))
                    return true;
            }

            return false;
        }

        Ranges to_source_ranges(Range range, const Maps &maps)
        {
            Ranges ranges;
            for (const auto &map : maps)
            {
                if (range && map.destination_range())
                {
                    ranges.push_back(map.to_source_range(
                        range & map.destination_range()));
                }
            }
            return ranges;
        }
    };

    const int humidity_to_location = maps_list.size() - 1;
    const int temperature_to_humidity = maps_list.size() - 2;
    Range seed_range;

    for (const auto &location_map : maps_list[humidity_to_location])
    {
        Range location_range = location_map.to_source_range(
            location_map.destination_range());

        if (Seeds().find_range(location_range, maps_list,
                               temperature_to_humidity, seed_range))
        {
            break;
        }
    }

    Number lowest_number = seed_range.start;

    for (const auto &maps : maps_list)
    {
        for (const auto &map : maps)
        {
            if (map.source <= lowest_number &&
                lowest_number < map.source + map.length)
            {
                lowest_number = map.destination + lowest_number - map.source;
                break;
            }
        }
    }

    return lowest_number;
}
