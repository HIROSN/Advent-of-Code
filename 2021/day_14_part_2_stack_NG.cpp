#include "main.h"

#include <functional>
#include <map>
#include <queue>
#include <stack>

const int final_step = 40;

uint64_t Answer(std::ifstream &file)
{
    std::string polymer;
    file >> polymer;
    std::string match, arrow, insert;
    std::map<std::string, char> rules;
    std::map<char, int> elements;

    for (auto &ch : polymer)
    {
        elements[ch]++;
    }

    while (file >> match >> arrow >> insert)
    {
        if (arrow == "->" && insert.size())
        {
            rules[match] = insert[0];
        }
    }

    struct PairType
    {
        std::string rule;
        int step = 0;
    };

    std::stack<PairType> pair_stack;

    for (int i = polymer.size() - 1; i > 0; i--)
    {
        PairType pair{polymer.substr(i - 1, 2), 0};
        pair_stack.push(pair);
    }

    while (true)
    {
        auto pair = pair_stack.top();
        pair_stack.pop();
        char insert = rules[pair.rule];
        elements[insert]++;

        if (pair.step + 1 < final_step)
        {
            pair_stack.push({insert + pair.rule.substr(1), pair.step + 1});
            pair_stack.push({pair.rule.substr(0, 1) + insert, pair.step + 1});
        }

        if (pair_stack.empty())
        {
            break;
        }
    }

    if (elements.size() > 0)
    {
        std::priority_queue<int> most_common;
        std::priority_queue<int, std::vector<int>, std::greater<int>> least_common;

        for (auto occurs : elements)
        {
            most_common.push(occurs.second);
            least_common.push(occurs.second);
        }

        return most_common.top() - least_common.top();
    }
    else
    {
        return 0;
    }
}
