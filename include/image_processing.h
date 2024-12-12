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

    std::vector<Pixel> neighbors(int four_or_eight)
    {
        std::vector<Pixel> pels;
        switch (four_or_eight)
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
        int labels = 0;

        while (true)
        {
            bool changed = false;

            for (int y = 0; y < size_y; y++)
            {
                for (int x = 0; x < size_x; x++)
                {
                    if (!labeled[y][x])
                    {
                        labeled[y][x] = ++labels;
                        if (renumber)
                            label_count[labeled[y][x]]++;
                    }

                    if (x < size_x - 1 && image[y][x + 1] == image[y][x] &&
                        labeled[y][x + 1] < labeled[y][x])
                    {
                        if (renumber)
                        {
                            label_count[labeled[y][x]]++;
                            label_count[labeled[y][x + 1]]--;
                        }
                        labeled[y][x + 1] = labeled[y][x];
                        changed = true;
                    }

                    if (y < size_y - 1 && image[y + 1][x] == image[y][x] &&
                        labeled[y + 1][x] < labeled[y][x])
                    {
                        if (renumber)
                        {
                            label_count[labeled[y][x]]++;
                            label_count[labeled[y + 1][x]]--;
                        }
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
                    {
                        labeled[y][x] = ++labels;
                        if (renumber)
                            label_count[labeled[y][x]]++;
                    }

                    if (x > 0 && image[y][x - 1] == image[y][x] &&
                        labeled[y][x - 1] < labeled[y][x])
                    {
                        if (renumber)
                        {
                            label_count[labeled[y][x]]++;
                            label_count[labeled[y][x - 1]]--;
                        }
                        labeled[y][x - 1] = labeled[y][x];
                        changed = true;
                    }

                    if (y > 0 && image[y - 1][x] == image[y][x] &&
                        labeled[y - 1][x] < labeled[y][x])
                    {
                        if (renumber)
                        {
                            label_count[labeled[y][x]]++;
                            label_count[labeled[y - 1][x]]--;
                        }
                        labeled[y - 1][x] = labeled[y][x];
                        changed = true;
                    }
                }
            }

            if (!changed)
                break;
        }

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
