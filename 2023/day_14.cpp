// #include <debug_print.h>
#include <main.h>

#include <utility>
#include <vector>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    std::vector<std::string> platform;
    std::string line;

    while (std::getline(file, line))
        platform.push_back(line);

    DPRINT(platform);
    const int size_x = line.size();
    const int size_y = platform.size();
    int total_load = 0;

    for (int y = 0; y < size_y; y++)
    {
        for (int x = 0; x < size_x; x++)
        {
            if (platform[y][x] == 'O')
            {
                int ny = y;

                while (ny > 0 && platform[ny - 1][x] == '.')
                {
                    std::swap(platform[ny - 1][x], platform[ny][x]);
                    ny--;
                }

                total_load += size_y - ny;
            }
        }
    }

    DPRINT(platform);
    return total_load;
}
