// #include <debug_print.h>
#include <main.h>

#include <vector>

int viewing_distance(std::vector<std::string> &trees,
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
    int highest_scenic_score = 0;

    for (int y = 0; y < size_y; y++)
    {
        for (int x = 0; x < size_x; x++)
        {
            int score = 1;
            score *= viewing_distance(trees, x, x, 0, y, 0, -1);
            score *= viewing_distance(trees, x, 0, -1, y, y, 0);
            score *= viewing_distance(trees, x, size_x - 1, +1, y, y, 0);
            score *= viewing_distance(trees, x, x, 0, y, size_y - 1, +1);

            if (score > highest_scenic_score)
            {
                highest_scenic_score = score;
            }
        }
    }

    return highest_scenic_score;
}

int viewing_distance(std::vector<std::string> &trees,
                     int sx, int ex, int dx,
                     int sy, int ey, int dy)
{
    int visible = 0;
    const int height_at_start = trees[sy][sx] - '0';

    for (int y = sy; true; y += dy)
    {
        for (int x = sx; true; x += dx)
        {
            const int height = trees[y][x] - '0';

            if (!(x == sx && y == sy))
            {
                visible++;

                if (height >= height_at_start)
                    return visible;
            }

            if (x == ex)
                break;
        }

        if (y == ey)
            break;
    }

    return visible;
}
