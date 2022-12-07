#include <main.h>

#include <sstream>

int get_total_size(std::ifstream &file, int &sum_of_total_sizes);

std::optional<uint64_t> Answer(std::ifstream &file)
{
    int sum_of_total_sizes = 0;
    get_total_size(file, sum_of_total_sizes);
    return sum_of_total_sizes;
}

int get_total_size(std::ifstream &file, int &sum_of_total_sizes)
{
    const int at_most = 100000;
    int total_size = 0, size;
    std::string line, prompt, command, dir;

    while (std::getline(file, line))
    {
        std::stringstream ss(line);

        if (ss.peek() == '$')
        {
            if (ss >> prompt &&
                ss >> command &&
                ss >> dir)
            {
                if (dir == "/")
                    continue;

                if (dir == "..")
                    break;

                total_size += get_total_size(file, sum_of_total_sizes);
            }
        }
        else
        {
            if (ss >> size)
            {
                total_size += size;
            }
        }
    }

    if (total_size <= at_most)
    {
        sum_of_total_sizes += total_size;
    }

    return total_size;
}
