#include "main.h"

#include <functional>
#include <queue>

const int final_step = 40;
static const char valid[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
constexpr int number_of_elements = sizeof(valid) / sizeof(valid[0]);
uint64_t elements[number_of_elements] = {};
char rules[number_of_elements][number_of_elements] = {};

bool is_valid(const std::string &str);
void pair_insertion(int step, char left, char right);

uint64_t Answer(std::ifstream &file)
{
    std::string polymer;
    file >> polymer;

    if (is_valid(polymer))
    {
        for (char element : polymer)
        {
            elements[element - 'A']++;
        }
    }

    std::string match, arrow, insert;

    while (file >> match >> arrow >> insert)
    {
        if (arrow == "->" && match.size() == 2 && insert.size() &&
            is_valid(match) && is_valid(insert))
        {
            rules[match[0] - 'A'][match[1] - 'A'] = insert[0];
        }
    }

    for (int i = 0; i < polymer.size() - 1; i++)
    {
        pair_insertion(1, polymer[i], polymer[i + 1]);
    }

    std::priority_queue<uint64_t> most_common;
    std::priority_queue<
        uint64_t,
        std::vector<uint64_t>, std::greater<uint64_t>>
        least_common;

    for (auto occurs : elements)
    {
        if (occurs)
        {
            most_common.push(occurs);
            least_common.push(occurs);
        }
    }

    return most_common.top() - least_common.top();
}

bool is_valid(const std::string &str)
{
    static std::string str_valid;

    if (str_valid.empty())
    {
        str_valid = valid;
    }

    for (auto ch : str)
    {
        if (str_valid.find(ch) == std::string::npos)
        {
            return false;
        }
    }
    return true;
}

void pair_insertion(int step, char left, char right)
{
    if (step <= final_step)
    {
        char insert = rules[left - 'A'][right - 'A'];

        if (insert)
        {
            elements[insert - 'A']++;
            pair_insertion(step + 1, left, insert);
            pair_insertion(step + 1, insert, right);
        }
    }
}
