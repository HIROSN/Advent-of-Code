#include <main.h>

#include <sstream>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    std::string line;
    int safe_reports = 0;

    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::optional<int> adjacent_level;
        std::optional<bool> all_increasing;
        bool unsafe = false;
        int level;

        while (ss >> level)
        {
            if (adjacent_level.has_value())
            {
                if ((unsafe = level == adjacent_level.value()))
                    break;

                int differ = level - adjacent_level.value();
                if ((unsafe = differ > 3 || differ < -3))
                    break;

                bool increasing = level > adjacent_level.value();
                if ((unsafe = all_increasing.has_value() && increasing != all_increasing.value()))
                    break;

                all_increasing = increasing;
            }
            adjacent_level = level;
        }
        safe_reports += unsafe ? 0 : 1;
    }

    return safe_reports;
}
