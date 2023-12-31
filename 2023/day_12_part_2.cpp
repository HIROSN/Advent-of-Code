// #define DTIMER_ON
// #include <debug_print.h>
#include <main.h>
#include <skip.h>

#include <sstream>
#include <unordered_map>
#include <vector>

struct Condition
{
    std::string springs;
    std::vector<int> groups;

    bool operator<(const Condition &right) const
    {
        return springs > right.springs;
    }

    Condition operator+(const Condition &right) const
    {
        auto copy = *this;
        copy.springs += '?';
        copy.springs += right.springs;
        copy.groups.insert(copy.groups.end(),
                           right.groups.begin(), right.groups.end());
        return copy;
    }

    std::string key() const
    {
        DCHECK(groups.size());
        std::stringstream key;
        key << springs << " " << groups[0];
        for (int i = 1; i < groups.size(); i++)
            key << "," << groups[i];
        return key.str();
    }

    int min() const
    {
        int min_required_space = 0;
        for (auto damaged : groups)
            min_required_space += damaged + 1;
        return min_required_space - 1;
    }

    friend std::ostream &operator<<(
        std::ostream &out, const Condition &condition);
};

std::unordered_map<std::string, uint64_t> cache;
#ifdef DPRINT_ON
uint64_t hits = 0;
#endif

uint64_t get_arrangements(Condition condition)
{
    auto it = cache.find(condition.key());
    if (it != cache.end())
    {
#ifdef DPRINT_ON
        hits++;
        // DPRINTX_ENDL(condition << " - " << it->second << " ("
        //                        << cache.size() << ")");
#endif
        return it->second;
    }

    int damaged = 0, i;

    for (i = 0; i < condition.springs.size(); i++)
    {
        if (damaged + condition.springs.size() - i < condition.min())
            return 0;

        if (condition.springs[i] == '?')
        {
            condition.springs[i] = '#';
            auto arrangements = get_arrangements(condition);

            condition.springs[i] = '.';
            arrangements += get_arrangements(condition);

            condition.springs[i] = '?';
            cache[condition.key()] = arrangements;
            // DPRINTX_ENDL(condition << " - "
            //                        << arrangements << " ("
            //                        << cache.size() << ")");
            return arrangements;
        }

        if (condition.springs[i] == '#')
        {
            if (++damaged > condition.groups[0])
                return 0;
            continue;
        }

        if (damaged)
            break;
    }

    condition.springs = condition.springs.substr(i);

    if (damaged)
    {
        if (damaged != condition.groups[0])
            return 0;

        condition.groups.erase(condition.groups.begin());
    }

    if (condition.groups.size())
    {
        if (condition.springs.size() < condition.groups[0])
            return 0;

        return get_arrangements(condition);
    }

    if (condition.springs.size())
    {
        if (condition.springs.find('#') != std::string::npos)
            return 0;
    }

    return 1;
}

std::optional<uint64_t> Answer(std::ifstream &file)
{
    std::vector<Condition> records;
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

        records.push_back(condition);
    }

    uint64_t sum_of_arrangements = 0;

    for (const auto &one : records)
    {
        auto five = one + one + one + one + one;

#ifdef DPRINT_ON
        static int c = 0;
        std::cout << "[" << ++c << "] " << five << std::endl;
        hits = 0;
        cache.clear();
        aoc::Timer timer;
#endif
        auto arrangements = get_arrangements(five);
        sum_of_arrangements += arrangements;

#ifdef DPRINT_ON
        auto elapsed = timer.elapsed();
        std::cout << one << " - " << arrangements << std::endl
                  << elapsed << " [sec.] "
                  << hits << "/" << cache.size() << std::endl;
#endif
    }

    return sum_of_arrangements;
}

std::ostream &operator<<(std::ostream &out, const Condition &condition)
{
#ifdef DPRINT_ON
    out << condition.key();
#endif
    return out;
}
