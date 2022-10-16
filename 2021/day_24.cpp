#include "main.h"

#include <algorithm>
#include <unordered_map>
#include <vector>

struct Difference
{
    int div_z = 1;
    int add_x = 0;
    int add_y = 0;
};

struct ModelNumber
{
    uint64_t number = 0;
    std::unordered_map<int64_t, int> unique_z;
};

const int mysterious_restrictions = 7;

int64_t run(int digit, int64_t prev_z, const Difference &diff);
std::vector<Difference> read_instructions(std::ifstream &file);

uint64_t Answer(std::ifstream &file)
{
    auto diffs = read_instructions(file);
    std::unordered_map<int64_t, ModelNumber> model{{0, {0}}};

    for (int i = 0; i < 14; i++)
    {
        auto prev_model = std::move(model);

        for (auto prev : prev_model)
        {
            const auto &prev_z = prev.first;
            const auto &prev_number = prev.second.number;
            const auto &prev_unique_z = prev.second.unique_z;

            for (int digit = 9; digit > 0; digit--)
            {
                auto z = run(digit, prev_z, diffs[i]);
                auto number = prev_number * 10 + digit;

                if (prev_unique_z.size() == mysterious_restrictions &&
                    prev_unique_z.find(z) == prev_unique_z.end())
                    continue;

                model[z].number = std::max(model[z].number, number);
                model[z].unique_z = prev_unique_z;
                model[z].unique_z[z]++;
            }
        }
    }

    return model[0].number;
}

int64_t run(int digit, int64_t prev_z, const Difference &diff)
{
    int64_t z = prev_z;
    z /= diff.div_z;

    if ((prev_z % 26) + diff.add_x != digit)
    {
        z *= 26;
        z += digit + diff.add_y;
    }

    return z;
}

std::vector<Difference> read_instructions(std::ifstream &file)
{
    const char *common_instructions[] = {
        "inp w",
        "mul x 0",
        "add x z",
        "mod x 26",
        "div z %d",
        "add x %d",
        "eql x w",
        "eql x 0",
        "mul y 0",
        "add y 25",
        "mul y x",
        "add y 1",
        "mul z y",
        "mul y 0",
        "add y w",
        "add y %d",
        "mul y x",
        "add z y",
    };

    std::vector<Difference> differences;
    Difference difference;

    std::unordered_map<std::string, int *> to_diff{
        {"div z %d", &difference.div_z},
        {"add x %d", &difference.add_x},
        {"add y %d", &difference.add_y}};

    for (int i = 0; i < 14; i++)
    {
        for (auto &instruction : common_instructions)
        {
            std::string line;

            if (!std::getline(file, line))
                return {};

            if (line == instruction)
                continue;

            if (sscanf(line.c_str(), instruction, to_diff[instruction]) == 1)
                continue;

            return {};
        }

        differences.push_back(difference);
    }

    return differences;
}
