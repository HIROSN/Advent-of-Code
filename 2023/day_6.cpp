#include <main.h>
#include <skip.h>

#include <list>
#include <sstream>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    auto getline = [&]() -> std::list<int>
    {
        std::list<int> list;
        std::string line;
        std::getline(file, line);
        std::stringstream ss(line);
        skip<std::string>(ss);
        int number;
        while (ss >> number)
            list.push_back(number);
        return list;
    };

    auto times = getline();
    auto distances = getline();
    int multiply_ways_to_win = 1;

    while (!times.empty() && !distances.empty())
    {
        auto time = times.front();
        auto distance = distances.front();
        int ways_to_win = 0;

        for (int msec = 1; msec < time; msec++)
        {
            int mm = time - msec;
            if (msec * mm > distance)
                ways_to_win++;
        }

        multiply_ways_to_win *= ways_to_win;
        times.pop_front();
        distances.pop_front();
    }

    return multiply_ways_to_win;
}
