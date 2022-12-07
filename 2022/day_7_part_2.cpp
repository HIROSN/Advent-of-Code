#include <main.h>

#include <functional>
#include <queue>
#include <sstream>

using reverse_priority_queue = std::priority_queue<int, std::vector<int>,
                                                   std::greater<int>>;

int get_total_size(std::ifstream &file, reverse_priority_queue &smallest);

std::optional<uint64_t> Answer(std::ifstream &file)
{
    const int total_disk_space = 70000000;
    const int unused_at_least = 30000000;
    reverse_priority_queue smallest;
    int unused = total_disk_space - get_total_size(file, smallest);

    while (!smallest.empty())
    {
        if (unused + smallest.top() >= unused_at_least)
        {
            return smallest.top();
        }
        smallest.pop();
    }

    return {};
}

int get_total_size(std::ifstream &file, reverse_priority_queue &smallest)
{
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

                total_size += get_total_size(file, smallest);
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

    smallest.push(total_size);
    return total_size;
}
