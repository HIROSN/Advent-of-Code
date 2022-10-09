// #include "debug_print.h"
#include "main.h"

#include <algorithm>
#include <bitset>
#include <sstream>
#include <vector>

using ImageType = std::vector<std::string>;
std::string image_enhancement_algorithm;

const int infinite = 3;

void enhance(ImageType &image);
int enhance_and_trim(ImageType &image, int trim = infinite);
void extend(int pels, ImageType &image);
void pixels_to_binary(std::string &line);
ImageType binary_to_pixels(const ImageType &image);
ImageType read_input_image(std::ifstream &file);

uint64_t Answer(std::ifstream &file)
{
    file >> image_enhancement_algorithm;
    pixels_to_binary(image_enhancement_algorithm);

    auto image = read_input_image(file);
    extend(4, image);
    enhance(image);
    return enhance_and_trim(image);
}

void enhance(ImageType &image)
{
    enhance_and_trim(image, 0);
}

int enhance_and_trim(ImageType &image, int trim)
{
    int pixels_lit = 0;
    ImageType enhanced;

    for (int y = 1; y < image.size() - 1; y++)
    {
        std::string row(image[0].size(), '0');

        if (y > trim && y < image.size() - trim - 1)
        {
            for (int x = trim + 1; x < image[0].size() - trim - 1; x++)
            {
                std::string nine_pixels = image[y - 1].substr(x - 1, 3) +
                                          image[y].substr(x - 1, 3) +
                                          image[y + 1].substr(x - 1, 3);
                std::bitset<9> nine_bits;
                std::stringstream(nine_pixels) >> nine_bits;
                row[x] = image_enhancement_algorithm[nine_bits.to_ulong()];
                pixels_lit += row[x] - '0';
            }
        }

        enhanced.push_back(row);
    }

    image = enhanced;
    DPRINT(binary_to_pixels(image));
    return pixels_lit;
}

void extend(int pels, ImageType &image)
{
    const std::string row(image[0].size(), '0');
    const std::string col(pels, '0');

    for (int i = 0; i < pels; i++)
    {
        image.insert(image.begin(), row);
        image.push_back(row);
    }

    for (auto &line : image)
    {
        line = col + line + col;
    }

    DPRINT(binary_to_pixels(image));
}

void pixels_to_binary(std::string &line)
{
    std::replace(line.begin(), line.end(), '.', '0');
    std::replace(line.begin(), line.end(), '#', '1');
}

ImageType binary_to_pixels(const ImageType &image)
{
    ImageType pixels;

    for (auto line : image)
    {
        std::replace(line.begin(), line.end(), '0', '.');
        std::replace(line.begin(), line.end(), '1', '#');
        pixels.push_back(line);
    }

    return pixels;
}

ImageType read_input_image(std::ifstream &file)
{
    ImageType image;
    std::string line;

    while (file >> line)
    {
        if (!line.empty())
        {
            pixels_to_binary(line);
            image.push_back(line);
        }
    }

    extend(2 + infinite, image);
    return image;
}
