#include <main.h>
#include <skip.h>

#include <sstream>
#include <unordered_map>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    using Bag = std::unordered_map<std::string, int>;
    const Bag the_bag{{"red", 12}, {"green", 13}, {"blue", 14}};

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

    auto possible = [](const Bag &bag) -> bool
    {
        for (auto it : bag)
        {
            if (it.second < 0)
                return false;
        }
        return true;
    };

    int sum_of_ids = 0;
    std::string line;

    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        int id, cubes;
        std::string str;
        Bag bag = the_bag;
        bool impossible = false;

        skip<std::string>(ss, 1);
        ss >> id;
        skip<char>(ss, 1);

        while (ss >> cubes >> str)
        {
            bag[color(str)] -= cubes;
            if (end_of_set(str))
            {
                if (!possible(bag))
                {
                    impossible = true;
                    break;
                }
                bag = the_bag;
            }
        }

        if (impossible)
            continue;
        sum_of_ids += id;
    }

    return sum_of_ids;
}
