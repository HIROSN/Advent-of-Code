#include <climits>
#include <fstream>
#include <iostream>
#include <sstream>
#include <queue>

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
    int sum = 0;
    std::queue<int> measurements;
    bool is_full = false;
    std::string line;

    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        int depth;
        ss >> depth;
        if (!is_full)
        {
            measurements.push(depth);
            sum += depth;
            if (measurements.size() < 3)
            {
                continue;
            }
            is_full = true;
        }
        else
        {
            sum -= measurements.front();
            measurements.push(depth);
            measurements.pop();
            sum += depth;
        }
        if (sum > previous)
        {
            increased++;
        }
        previous = sum;
    }

    std::cout << increased << std::endl;
    file.close();
    return 0;
}
