#include "main.h"

#include <functional>
#include <mutex>
#include <queue>
#include <thread>

const int final_step = 40;
static const char valid[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
constexpr int number_of_elements = sizeof(valid) / sizeof(valid[0]);
uint64_t elements[number_of_elements] = {};
char rules[number_of_elements][number_of_elements] = {};

bool is_valid(const std::string &str);
void pair_insertion(int step, char left, char right);

const int number_of_threads = 2;
static std::mutex mutex;
static thread_local uint64_t elements_per_thread[number_of_elements] = {};
void pair_insertion_thread_proc(char left, char right);

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

    std::queue<std::thread> threads;

    for (int i = 0; i < polymer.size() - 1; i++)
    {
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
            elements_per_thread[insert - 'A']++;
            pair_insertion(step + 1, left, insert);
            pair_insertion(step + 1, insert, right);
        }
    }
}

void pair_insertion_thread_proc(char left, char right)
{
    pair_insertion(1, left, right);
    std::lock_guard<std::mutex> lock(mutex);

    for (int i = 0; i < number_of_elements; i++)
    {
        elements[i] += elements_per_thread[i];
    }
}
