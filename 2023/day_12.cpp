#include <main.h>
#include <skip.h>

#include <algorithm>
#include <queue>
#include <sstream>
#include <vector>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    struct Condition
    {
        std::string springs;
        std::vector<int> groups;

        bool has_unknown(size_t &unknown) const
        {
            unknown = springs.find('?');
            return unknown != std::string::npos;
        }

        bool valid() const
        {
            size_t unknown;
            bool incomplete = has_unknown(unknown);
            auto known = springs.substr(0, unknown);

            for (int i = 0; i < groups.size(); i++)
            {
                size_t size = groups[i];
                auto damaged = known.find('#');

                if (damaged == std::string::npos)
                    return incomplete;

                if (incomplete)
                    size = std::min(size, known.size() - damaged);

                if (known.substr(damaged, size) != std::string(size, '#'))
                    return false;

                known = known.substr(damaged + size);

                if (known.size() && known[0] == '#')
                    return false;
            }

            return known.find('#') == std::string::npos;
        }
    };

    std::queue<Condition> records;
    std::string line;

    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        Condition condition;
        int number;

        ss >> condition.springs;
        while (ss >> number)
        {
            condition.groups.push_back(number);
            skip<char>(ss);
        }

        CHECK(condition.valid());
        records.push(condition);
    }

    int arrangements = 0;

    while (!records.empty())
    {
        auto condition = records.front();
        records.pop();

        size_t unknown;
        if (condition.has_unknown(unknown))
        {
            auto condition_copy = condition;
            condition_copy.springs[unknown] = '.';
            if (condition_copy.valid())
                records.push(condition_copy);

            condition.springs[unknown] = '#';
            if (condition.valid())
                records.push(condition);
        }
        else
        {
            arrangements++;
        }
    }

    return arrangements;
}
