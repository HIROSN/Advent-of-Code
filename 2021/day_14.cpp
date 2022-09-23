#include "main.h"

#include <functional>
#include <map>
#include <queue>
#include <sstream>

const int final_step = 10;
std::map<char, int> elements;

void AddElement(int step, std::stringstream &ss, char element);

uint64_t Answer(std::ifstream &file)
{
    std::string polymer;
    file >> polymer;
    std::string match, arrow, insert;
    std::map<std::string, char> rules;

    while (file >> match >> arrow >> insert)
    {
        if (arrow == "->" && insert.size())
        {
            rules[match] = insert[0];
        }
    }

    for (int step = 1; step <= final_step; step++)
    {
        std::stringstream ss;
        char element;

        for (int i = 0; i < polymer.size() - 1; i++)
        {
            AddElement(step, ss, polymer[i]);
            std::string pair = polymer.substr(i, 2);

            if (rules.find(pair) != rules.end())
            {
                AddElement(step, ss, rules[pair]);
            }
        }

        AddElement(step, ss, polymer[polymer.size() - 1]);
        polymer = ss.str();
    }

    std::priority_queue<int> most_common;
    std::priority_queue<int, std::vector<int>, std::greater<int>> least_common;

    for (auto occurs : elements)
    {
        most_common.push(occurs.second);
        least_common.push(occurs.second);
    }

    return most_common.top() - least_common.top();
}

void AddElement(int step, std::stringstream &ss, char element)
{
    ss << element;

    if (step == final_step)
    {
        elements[element]++;
    }
}
