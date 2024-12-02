#include <main.h>

#include <cstdlib>
#include <queue>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    int left, right;
    std::priority_queue<int> left_list, right_list;
    uint64_t total_distance = 0;

    while (file >> left >> right)
    {
        left_list.push(left);
        right_list.push(right);
    }

    while (!left_list.empty())
    {
        total_distance += std::abs(left_list.top() - right_list.top());
        left_list.pop();
        right_list.pop();
    }

    return total_distance;
}
