// #include <debug_print.h>
#include <main.h>

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

    std::vector<std::vector<int>> labeled(
        size_y, std::vector<int>(size_x, 0));
    int labels = 0;

    while (true)
    {
        bool changed = false;

        for (int y = 0; y < size_y; y++)
        {
            for (int x = 0; x < size_x; x++)
            {
                if (!labeled[y][x])
                    labeled[y][x] = ++labels;

                if (x < size_x - 1 && garden[y][x + 1] == garden[y][x] &&
                    labeled[y][x + 1] < labeled[y][x])
                {
                    labeled[y][x + 1] = labeled[y][x];
                    changed = true;
                }
                if (y < size_y - 1 && garden[y + 1][x] == garden[y][x] &&
                    labeled[y + 1][x] < labeled[y][x])
                {
                    labeled[y + 1][x] = labeled[y][x];
                    changed = true;
                }
            }
        }

        if (!changed)
            break;

        for (int y = size_y - 1; y >= 0; y--)
        {
            for (int x = size_x - 1; x >= 0; x--)
            {
                if (!labeled[y][x])
                    labeled[y][x] = ++labels;

                if (x > 0 && garden[y][x - 1] == garden[y][x] &&
                    labeled[y][x - 1] < labeled[y][x])
                {
                    labeled[y][x - 1] = labeled[y][x];
                    changed = true;
                }
                if (y > 0 && garden[y - 1][x] == garden[y][x] &&
                    labeled[y - 1][x] < labeled[y][x])
                {
                    labeled[y - 1][x] = labeled[y][x];
                    changed = true;
                }
            }
        }

        if (!changed)
            break;
    }

    DPRINT(labeled);
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

            if (get_label(x, y - 1) != label)
            {
                perimeter[label]++;

                if (x < size_x - 1 &&
                    get_label(x + 1, y) == label &&
                    get_label(x + 1, y - 1) != label)
                {
                    perimeter[label]--;
                }
            }

            if (get_label(x + 1, y) != label)
            {
                perimeter[label]++;

                if (y < size_y - 1 &&
                    get_label(x, y + 1) == label &&
                    get_label(x + 1, y + 1) != label)
                {
                    perimeter[label]--;
                }
            }

            if (get_label(x, y + 1) != label)
            {
                perimeter[label]++;

                if (x < size_x - 1 &&
                    get_label(x + 1, y) == label &&
                    get_label(x + 1, y + 1) != label)
                {
                    perimeter[label]--;
                }
            }

            if (get_label(x - 1, y) != label)
            {
                perimeter[label]++;

                if (y < size_y - 1 &&
                    get_label(x, y + 1) == label &&
                    get_label(x - 1, y + 1) != label)
                {
                    perimeter[label]--;
                }
            }
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
