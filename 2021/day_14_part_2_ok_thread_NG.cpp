#include "main.h"

#include <cmath>
#include <cstring>
#include <functional>
#include <map>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>

constexpr int final_step = 40;
constexpr int cache_at_steps = 10;
constexpr int cache_passes = 2;
constexpr int cache_string_size = std::pow(2, cache_at_steps) + 1;
std::map<std::pair<char, char>, char> rules;
static const char valid[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
constexpr int number_of_elements = sizeof(valid) / sizeof(valid[0]);
uint64_t elements[number_of_elements] = {};

struct PolymerInsertionType
{
    char polymer[cache_string_size + 1] = {};
    uint64_t elements[number_of_elements] = {};
};

PolymerInsertionType
    polymer_insertion_cache[number_of_elements][number_of_elements] = {};

bool is_cached(char left, char right);
PolymerInsertionType pair_insertion(const int steps, char left, char right);
bool is_valid(const std::string &str);
void add_elements(uint64_t *add_to, uint64_t *insertion);
void print_progress(int i, const std::string &polymer);

const int number_of_threads = 2;
static std::mutex mutex;
static thread_local uint64_t elements_per_thread[number_of_elements] = {};
void pair_insertion_thread_proc(char left, char right);

uint64_t Answer(std::ifstream &file)
{
    std::string polymer, match, arrow, insert;
    file >> polymer;

    if (is_valid(polymer))
    {
        for (auto ch : polymer)
        {
            elements[ch - 'A']++;
        }
    }

    while (file >> match >> arrow >> insert)
    {
        if (arrow == "->" && match.size() >= 2 && insert.size() &&
            is_valid(match) && is_valid(insert))
        {
            rules[std::make_pair(match[0], match[1])] = insert[0];
        }
    }

    for (int pass = 1; pass <= cache_passes; pass++)
    {
        std::string new_polymer;

        for (int i = 0; i < polymer.size() - 1; i++)
        {
            auto polymer_insertion = pair_insertion(
                cache_at_steps, polymer[i], polymer[i + 1]);

            add_elements(elements, polymer_insertion.elements);

            if (new_polymer.empty())
            {
                new_polymer = std::string(polymer_insertion.polymer);
            }
            else
            {
                new_polymer +=
                    std::string(polymer_insertion.polymer).substr(1);
            }
        }

        polymer = new_polymer;
    }

    if (final_step > cache_at_steps)
    {
        std::queue<std::thread> threads;

        for (int i = 0; i < polymer.size() - 1; i++)
        {
            print_progress(i, polymer);
            threads.emplace(pair_insertion_thread_proc, polymer[i], polymer[i + 1]);

            if (threads.size() >= number_of_threads)
            {
                threads.front().join();
                threads.pop();
            }
        }

        while (!threads.empty())
        {
            threads.front().join();
            threads.pop();
        }
    }

    std::priority_queue<uint64_t> most_common;
    std::priority_queue<uint64_t, std::vector<uint64_t>, std::greater<uint64_t>>
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

bool is_cached(char left, char right)
{
    return polymer_insertion_cache[left - 'A'][right - 'A'].polymer[0];
}

PolymerInsertionType pair_insertion(const int steps, char left, char right)
{
    if (steps == cache_at_steps && is_cached(left, right))
    {
        return polymer_insertion_cache[left - 'A'][right - 'A'];
    }

    std::string polymer;
    auto pair = std::make_pair(left, right);
    PolymerInsertionType polymer_insertion;

    if (rules.find(pair) != rules.end())
    {
        char insert = rules[pair];
        polymer_insertion.elements[insert - 'A']++;

        if (steps > 1)
        {
            auto pi_left = pair_insertion(steps - 1, left, insert);
            auto pi_right = pair_insertion(steps - 1, insert, right);
            polymer = std::string(pi_left.polymer);
            polymer += std::string(pi_right.polymer).substr(1);
            add_elements(polymer_insertion.elements, pi_left.elements);
            add_elements(polymer_insertion.elements, pi_right.elements);
        }
        else
        {
            polymer = left;
            polymer += insert;
            polymer += right;
        }
    }
    else
    {
        polymer = left;
        polymer += right;
    }

    std::strcpy(polymer_insertion.polymer, polymer.c_str());

    if (steps == cache_at_steps)
    {
        polymer_insertion_cache[left - 'A'][right - 'A'] = polymer_insertion;
    }

    return polymer_insertion;
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

void add_elements(uint64_t *add_to, uint64_t *insertion)
{
    for (int i = 0; i < number_of_elements; i++)
    {
        if (insertion[i])
        {
            add_to[i] += insertion[i];
        }
    }
}

void print_progress(int i, const std::string &polymer)
{
    if (polymer.size() > 10 && (i % (polymer.size() / 10)) == 0)
    {
        std::cout << "Completed " << (i * 100 / polymer.size()) + 1 << "% - ";
        int cache_size = 0;

        for (int l = 0; l < number_of_elements; l++)
        {
            for (int r = 0; r < number_of_elements; r++)
            {
                cache_size += polymer_insertion_cache[l][r].polymer[0] ? 1 : 0;
            }
        }

        std::cout << "Cached " << cache_size * 100 / rules.size() << "%"
                  << std::endl;
    }
}

void pair_insertion_thread_proc(char left, char right)
{
    auto polymer_insertion = pair_insertion(cache_at_steps, left, right);
    {
        std::lock_guard<std::mutex> lock(mutex);
        add_elements(elements, polymer_insertion.elements);
    }

    for (int j = 0;
         j < cache_string_size && polymer_insertion.polymer[j + 1];
         j++)
    {
        char &left = polymer_insertion.polymer[j];
        char &right = polymer_insertion.polymer[j + 1];

        if (is_cached(left, right))
        {
            std::lock_guard<std::mutex> lock(mutex);
            add_elements(elements,
                         polymer_insertion_cache[left - 'A'][right - 'A'].elements);
        }
        else
        {
            auto sub_polymer_insertion = pair_insertion(
                cache_at_steps,
                polymer_insertion.polymer[j],
                polymer_insertion.polymer[j + 1]);

            std::lock_guard<std::mutex> lock(mutex);
            add_elements(elements, sub_polymer_insertion.elements);
        }
    }
}
