#include <climits>
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
    int previous = INT_MAX;
    for (auto depth : measurements)
    {
        if (depth > previous)
        {
            increased++;
        }
        previous = depth;
    }

    std::cout << increased << std::endl;
    return 0;
}
