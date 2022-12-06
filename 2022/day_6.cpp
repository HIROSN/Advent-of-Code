#include <main.h>

#include <map>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    const int marker_size = 4;
    std::string line;

    while (std::getline(file, line))
    {
        std::map<char, int> characters;

        for (int i = 0; i < line.size(); i++)
        {
            characters[line[i]]++;

            if (i < marker_size)
            {
                continue;
            }

            if (--characters[line[i - marker_size]] == 0)
            {
                characters.erase(line[i - marker_size]);
            }

            if (characters.size() == marker_size)
            {
                std::cout << (i + 1) << std::endl;
                break;
            }
        }
    }

    return {};
}
