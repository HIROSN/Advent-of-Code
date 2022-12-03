#include <main.h>

#include <map>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    std::string rucksack;
    int priorities = 0;

    while (std::getline(file, rucksack))
    {
        std::map<char, int> type_count;
        const int rucksack_size = rucksack.size();

        for (int i = 0; i < rucksack_size; i++)
        {
            if (i < rucksack_size / 2)
            {
                type_count[rucksack[i]]++;
                continue;
            }

            if (type_count[rucksack[i]] > 0)
            {
                priorities += rucksack[i] - 'a' > 0 ? rucksack[i] - 'a' + 1
                                                    : rucksack[i] - 'A' + 27;
                break;
            }
        }
    }

    return priorities;
}
