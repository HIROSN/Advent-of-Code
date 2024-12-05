#include <main.h>
#include <skip.h>

#include <sstream>
#include <vector>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    std::string line;
    bool rules[100][100] = {};

    while (std::getline(file, line))
    {
        if (line.empty())
            break;

        std::stringstream ss(line);
        int left, right;
        ss >> left;
        skip<char>(ss);
        ss >> right;
        rules[left][right] = true;
    }

    std::vector<std::vector<int>> updates;

    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::vector<int> update;
        int page;

        while (ss >> page)
        {
            update.push_back(page);
            skip<char>(ss);
        }

        updates.push_back(update);
    }

    int add_up_middle_page_number = 0;

    for (auto update : updates)
    {
        bool correct = true;

        for (int il = 0; correct && il < update.size() - 1; il++)
            for (int ir = il + 1; correct && ir < update.size(); ir++)
                correct = !rules[update[ir]][update[il]];

        if (correct)
            continue;

        while (!correct)
        {
            correct = true;
            for (int il = 0; correct && il < update.size() - 1; il++)
            {
                for (int ir = il + 1; correct && ir < update.size(); ir++)
                {
                    correct = !rules[update[ir]][update[il]];
                    if (!correct)
                        std::swap(update[ir], update[il]);
                }
            }
        }

        if (correct)
            add_up_middle_page_number += update[update.size() / 2];
    }

    return add_up_middle_page_number;
}
