// #include <debug_print.h>
#include <main.h>
#include <image_processing.h>

#include <unordered_map>
#include <vector>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    std::vector<std::string> garden;
    std::string line;

    while (std::getline(file, line))
        garden.push_back(line);

    DPRINT(garden);
    const int size_x = line.size();
    const int size_y = garden.size();
    auto labeled = aoc::labeling(garden);
    std::unordered_map<int, uint64_t> area;
    std::unordered_map<int, uint64_t> perimeter;
    std::unordered_map<int, char> label_plants_map;

    auto get_label = [&](int x, int y) -> int
    {
        if (x >= 0 && x < size_x && y >= 0 && y < size_y)
            return labeled[y][x];
        else
            return 0;
    };

    for (int y = 0; y < size_y; y++)
    {
        for (int x = 0; x < size_x; x++)
        {
            const int &label = get_label(x, y);
            label_plants_map[label] = garden[y][x];
            area[label]++;

            for (auto n : aoc::neighbors(4))
                if (get_label(x + n.x, y + n.y) != label)
                    perimeter[label]++;
        }
    }

    uint64_t total_price = 0;

    for (auto it : area)
    {
        const int &label = it.first;
        auto price = area[label] * perimeter[label];
        total_price += price;
        DPRINTX_ENDL(label_plants_map[label]
                     << " : " << area[label]
                     << " * " << perimeter[label]
                     << " = " << price)
    }

    return total_price;
}
