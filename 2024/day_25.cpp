// #include <debug_print.h>
#include <main.h>

#include <vector>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    using Schematic = std::vector<std::string>;
    using Schematics = std::vector<Schematic>;

    Schematics locks, keys;
    std::optional<bool> is_lock;
    Schematic schematic;
    std::string line;

    while (std::getline(file, line))
    {
        if (line.size() == 0)
        {
            if (is_lock.value())
                locks.push_back(schematic);
            else
                keys.push_back(schematic);

            is_lock.reset();
            schematic.clear();
            continue;
        }

        if (!is_lock.has_value())
            is_lock = line == "#####";
        schematic.push_back(line);
    }

    if (is_lock.value())
        locks.push_back(schematic);
    else
        keys.push_back(schematic);

    DPRINT2(locks.size(), keys.size());
    int pairs_fit = 0;

    for (const auto &lock : locks)
    {
        for (const auto &key : keys)
        {
            bool overlap = false;
            for (int y = 0; !overlap && y < lock.size(); y++)
                for (int x = 0; !overlap && x < lock[y].size(); x++)
                    overlap = lock[y][x] == '#' && key[y][x] == '#';

            if (!overlap)
                pairs_fit++;
        }
    }

    return pairs_fit;
}
