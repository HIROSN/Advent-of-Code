#include <main.h>

#include <queue>
#include <sstream>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    std::priority_queue<int> most_calories;
    int calories = 0;
    std::string line;

    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        int calorie;

        if (ss >> calorie)
        {
            calories += calorie;
        }
        else
        {
            most_calories.push(calories);
            calories = 0;
        }
    }

    most_calories.push(calories);
    return most_calories.top();
}
