#include <climits>
#include <fstream>
#include <iostream>
#include <sstream>

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

    int increased = 0;
    int previous = INT_MAX;
    std::string line;
    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        int depth;
        ss >> depth;
        if (depth > previous)
        {
            increased++;
        }
        previous = depth;
    }
    std::cout << increased << std::endl;

    file.close();
    return 0;
}
