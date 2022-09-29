#include "main.h"

#include <functional>
#include <queue>

constexpr int final_step = 40;
static const char valid[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
constexpr int number_of_elements = sizeof(valid) / sizeof(valid[0]);
uint64_t elements[number_of_elements] = {};
char rules[number_of_elements][number_of_elements] = {};

struct CacheType
{
    uint64_t elements[number_of_elements] = {};
    std::string polymer;
};

constexpr int cache_size = final_step / 2;
CacheType cache[number_of_elements][number_of_elements] = {};

std::string pair_insertion(int step, char left, char right,
                           uint64_t *new_elements);

void add_elements(const uint64_t *insertion);
bool is_valid(const std::string &str);

uint64_t Answer(std::ifstream &file)
{
    std::string polymer;
    file >> polymer;

    if (is_valid(polymer))
    {
        for (char element : polymer)
        {
            elements[element - valid[0]]++;
        }
    }

    std::string match, arrow, insert;

    while (file >> match >> arrow >> insert)
    {
        if (arrow == "->" && match.size() == 2 && insert.size() &&
            is_valid(match) && is_valid(insert))
        {
            rules[match[0] - valid[0]][match[1] - valid[0]] = insert[0];
        }
    }

    for (int il = 0; il < number_of_elements; il++)
    {
        for (int ir = 0; ir < number_of_elements; ir++)
        {
            if (rules[il][ir])
            {
                cache[il][ir].polymer = pair_insertion(
                    1, valid[il], valid[ir], cache[il][ir].elements);
            }
        }
    }

    for (int i1 = 0; i1 < polymer.size() - 1; i1++)
    {
        int il1 = polymer[i1] - valid[0];
        int ir1 = polymer[i1 + 1] - valid[0];
        add_elements(cache[il1][ir1].elements);

        for (int i2 = 0; i2 < cache[il1][ir1].polymer.size() - 1; i2++)
        {
            int il2 = cache[il1][ir1].polymer[i2] - valid[0];
            int ir2 = cache[il1][ir1].polymer[i2 + 1] - valid[0];
            add_elements(cache[il2][ir2].elements);
        }
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

std::string pair_insertion(int step, char left, char right,
                           uint64_t *new_elements)
{
    std::string polymer;
    char insert = rules[left - valid[0]][right - valid[0]];

    if (insert)
    {
        new_elements[insert - valid[0]]++;

        if (step < cache_size)
        {
            polymer = pair_insertion(step + 1, left, insert, new_elements);
            polymer += pair_insertion(step + 1, insert, right, new_elements)
                           .substr(1);
        }
        else
        {
            polymer = left;
            polymer += insert;
            polymer += right;
        }
    }

    return polymer;
}

void add_elements(const uint64_t *insertion)
{
    for (int i = 0; i < number_of_elements; i++)
    {
        if (insertion[i])
        {
            elements[i] += insertion[i];
        }
    }
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
