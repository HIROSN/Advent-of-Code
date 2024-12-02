#include <main.h>

#include <unordered_map>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    int left, right;
    std::unordered_map<int, int> left_list, right_list;
    uint64_t similarity_score = 0;

    while (file >> left >> right)
    {
        left_list[left]++;
        right_list[right]++;
    }

    for (auto it : left_list)
    {
        const int &number = it.first;
        uint64_t score = number * right_list[number];
        similarity_score += score * it.second;
    }

    return similarity_score;
}
