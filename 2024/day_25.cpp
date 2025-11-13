// #include <debug_print.h>
#include <main.h>

#include <vector>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    using Schematic = std::vector<int>;
    using Schematics = std::vector<Schematic>;

    Schematics locks, keys;
    std::optional<bool> is_lock;
    Schematic schematic(5, -1);
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
            schematic = Schematic(5, -1);
            continue;
        }

        if (!is_lock.has_value())
            is_lock = line == "#####";

        for (int i = 0; i < schematic.size(); i++)
            schematic[i] += line[i] == '#';
    }

    if (is_lock.value())
        locks.push_back(schematic);
    else
        keys.push_back(schematic);

    DPRINT(locks);
    DPRINT(keys);
    DPRINT2(locks.size(), keys.size());
    DPRINT_ENDL();
    int pairs_fit = 0;

    for (const auto &lock : locks)
    {
        for (const auto &key : keys)
        {
            bool overlap = false;
            for (int i = 0; !overlap && i < lock.size(); i++)
                overlap = lock[i] + key[i] > 5;

            if (!overlap)
                pairs_fit++;
        }
    }

    return pairs_fit;
}
