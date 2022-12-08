// #include <debug_print.h>
#include <main.h>

#include <vector>

int flag_visible(std::vector<std::string> &trees,
                 int sx, int ex, int dx,
                 int sy, int ey, int dy);

std::optional<uint64_t> Answer(std::ifstream &file)
{
    std::vector<std::string> trees;
    std::string line;

    while (std::getline(file, line))
    {
        trees.push_back(line);
    }

    DPRINT(trees);
    const int size_x = line.size();
    const int size_y = trees.size();
    int visible = 0;

    for (int y = 0; y < size_y; y++)
    {
        visible += flag_visible(trees, 0, size_x - 1, +1, y, y, 0);
        visible += flag_visible(trees, size_x - 1, 0, -1, y, y, 0);
    }

    for (int x = 0; x < size_x; x++)
    {
        visible += flag_visible(trees, x, x, 0, 0, size_y - 1, +1);
        visible += flag_visible(trees, x, x, 0, size_y - 1, 0, -1);
    }

    DPRINT(trees);
    return visible;
}

int flag_visible(std::vector<std::string> &trees,
                 int sx, int ex, int dx,
                 int sy, int ey, int dy)
{
    int flagged_visible = 0;
    const char mask = 0x3F;
    const char flagged = 0x40;
    const int max_height = 9;
    int highest = -1;

    for (int y = sy; highest < max_height; y += dy)
    {
        for (int x = sx; highest < max_height; x += dx)
        {
            const int height = (trees[y][x] & mask) - '0';
            char &tree = trees[y][x];

            if (height > highest)
            {
                if (!(tree & flagged))
                {
                    tree |= flagged;
                    flagged_visible++;
                }
                highest = height;
            }

            if (x == ex)
                break;
        }

        if (y == ey)
            break;
    }

    return flagged_visible;
}
