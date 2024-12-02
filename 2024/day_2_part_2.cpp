#include <main.h>

#include <sstream>
#include <vector>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    std::string line;
    int safe_reports = 0;

    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::vector<int> levels;
        int level;

        while (ss >> level)
        {
            levels.push_back(level);
        }

        auto is_safe = [](const std::vector<int> &levels) -> bool
        {
            std::optional<int> adjacent_level;
            std::optional<bool> all_increasing;
            bool unsafe = false;

            for (auto level : levels)
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
            return !unsafe;
        };

        if (is_safe(levels))
        {
            safe_reports++;
        }
        else
        {
            for (size_t i = 0; i < levels.size(); i++)
            {
                auto levels_copy = levels;
                levels_copy.erase(levels_copy.begin() + i);
                if (is_safe(levels_copy))
                {
                    safe_reports++;
                    break;
                }
            }
        }
    }

    return safe_reports;
}
