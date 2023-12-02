#include <main.h>
#include <skip.h>

#include <sstream>
#include <unordered_map>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    using Bag = std::unordered_map<std::string, int>;

    auto color = [](const std::string &str) -> std::string
    {
        auto it = str.rbegin();
        if (it != str.rend() && (*it == ',' || *it == ';'))
        {
            return str.substr(0, str.length() - 1);
        }
        return str;
    };

    auto end_of_set = [](const std::string &str) -> bool
    {
        auto it = str.rbegin();
        return !(it != str.rend() && *it == ',');
    };

    int sum_of_power = 0;
    std::string line;

    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        int id, cubes, power = 1;
        std::string str;
        Bag bag, fewest;

        skip<std::string>(ss, 1);
        ss >> id;
        skip<char>(ss, 1);

        while (ss >> cubes >> str)
        {
            bag[color(str)] += cubes;
            if (end_of_set(str))
            {
                for (auto it : bag)
                {
                    if (it.second > fewest[it.first])
                        fewest[it.first] = it.second;
                }
                bag.clear();
            }
        }

        for (auto it : fewest)
        {
            power *= it.second;
        }
        sum_of_power += power;
    }

    return sum_of_power;
}
