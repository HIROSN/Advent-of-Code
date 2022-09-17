#include <fstream>
#include <iostream>
#include <iterator>
#include <vector>

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        std::cout << "Usage: " << argv[0] << " input" << std::endl;
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file.is_open())
    {
        return 2;
    }
    std::vector<int> measurements(std::istream_iterator<int>(file), {});
    file.close();

    int increased = 0;
    const int sliding_window_size = 3;
    if (measurements.size() > sliding_window_size)
    {
        for (size_t i = sliding_window_size; i < measurements.size(); i++)
        {
            if (measurements[i] > measurements[i - sliding_window_size])
            {
                increased++;
            }
        }
    }

    std::cout << increased << std::endl;
    return 0;
}
