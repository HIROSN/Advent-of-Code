#include <main.h>

#include <map>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    int priorities = 0;

    while (file.good())
    {
        std::map<char, int> type_flag;

        for (int i = 1; i <= 3; i++)
        {
            std::string rucksack;
            std::getline(file, rucksack);

            for (auto type : rucksack)
            {
                if (i < 3)
                {
                    type_flag[type] |= i;
                    continue;
                }

                if (type_flag[type] == 3)
                {
                    priorities += type - 'a' > 0 ? type - 'a' + 1
                                                 : type - 'A' + 27;
                    break;
                }
            }
        }
    }

    return priorities;
}
