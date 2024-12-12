#pragma once

#include <map>
#include <string>
#include <utility>
#include <vector>

#ifndef Number
#define Number int
#endif

#ifndef X
#define X 0
#endif

#ifndef Y
#define Y 1
#endif

namespace aoc
{
    using Image = std::vector<std::string>;
    using NumberedImage = std::vector<std::vector<Number>>;

    struct Pixel
    {
        Number x = 0;
        Number y = 0;

        std::pair<Number, Number> as_pair()
        {
            return {x, y};
        }
    };

    std::vector<Pixel> neighbors(int number_of_neighbors)
    {
        std::vector<Pixel> pels;
        switch (number_of_neighbors)
        {
        case 4:
            return {{0, -1}, {1, 0}, {0, 1}, {-1, 0}};
        case 8:
            return {{-1, -1},
                    {0, -1},
                    {1, -1},
                    {1, 0},
                    {1, 1},
                    {0, 1},
                    {-1, 1},
                    {-1, 0}};
        case 2:
            return {{1, 0}, {0, 1}};
        case -2:
            return {{-1, 0}, {0, -1}};
        }
        return {};
    }

    NumberedImage labeling(const NumberedImage &image, bool renumber = true)
    {
        if (image.empty())
            return {};

        const int size_x = image[0].size();
        const int size_y = image.size();
        NumberedImage labeled(size_y, std::vector<Number>(size_x, 0));
        std::map<Number, unsigned int> label_count;
        bool changed = true;
        int labels = 0;

        auto scan_for_change = [&](const Pixel &start,
                                   const Pixel &end,
                                   const int step)
        {
            bool changed = false;

            for (int y = start.y; y != end.y; y += step)
            {
                for (int x = start.x; x != end.x; x += step)
                {
                    if (!labeled[y][x])
                    {
                        labeled[y][x] = ++labels;
                        if (renumber)
                            label_count[labeled[y][x]]++;
                    }

                    for (auto n : neighbors(step * 2))
                    {
                        const int nx = x + n.x;
                        const int ny = y + n.y;

                        if (nx >= 0 && nx < size_x && ny >= 0 && ny < size_y &&
                            image[ny][nx] == image[y][x] && labeled[ny][nx] < labeled[y][x])
                        {
                            if (renumber)
                            {
                                label_count[labeled[y][x]]++;
                                label_count[labeled[ny][nx]]--;
                            }
                            labeled[ny][nx] = labeled[y][x];
                            changed = true;
                        }
                    }
                }
            }

            return changed;
        };

        while (changed)
            changed = scan_for_change({0, 0}, {size_x, size_y}, +1) &&
                      scan_for_change({size_x - 1, size_y - 1}, {-1, -1}, -1);

        if (renumber)
        {
            std::map<Number, Number> new_label;
            labels = 0;

            for (auto it : label_count)
                if (it.first && it.second)
                    new_label[it.first] = ++labels;

            for (int y = 0; y < size_y; y++)
                for (int x = 0; x < size_x; x++)
                    if (labeled[y][x])
                        labeled[y][x] = new_label[labeled[y][x]];
        }

        return labeled;
    }

    NumberedImage labeling(const Image &image,
                           char background = '.',
                           bool renumber = true)
    {
        if (image.empty())
            return {};

        const int size_x = image[0].size();
        const int size_y = image.size();
        NumberedImage numbered(size_y, std::vector<Number>(size_x, 0));

        for (int y = 0; y < size_y; y++)
            for (int x = 0; x < size_x; x++)
                if (image[y][x] != background)
                    numbered[y][x] = static_cast<Number>(image[y][x]);

        return labeling(numbered, renumber);
    }
}
