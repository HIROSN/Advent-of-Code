#pragma once

#include <cstdint>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include <timer.h>

std::vector<std::string_view> args;

std::optional<uint64_t> Answer(std::ifstream &file);

int main(int argc, char *argv[])
{
    std::ifstream file;

    if (argc > 1)
    {
        file.open(argv[1]);

        if (!file.is_open())
        {
            return 1;
        }
    }

    for (int i = 0; i < argc; i++)
    {
        args.emplace_back(argv[i]);
    }

#ifdef DPRINT_ON
    aoc::Timer timer;
#endif
    auto answer = Answer(file);
#ifdef DPRINT_ON
    std::cout << timer.elapsed() << " [sec.]" << std::endl;
#endif

    if (answer)
    {
        std::cout << answer.value() << std::endl;
    }

    file.close();
    return 0;
}

#ifndef DPRINT_ON
#define DPRINT_ENDL()
#define DPRINT(x)
#define DPRINT2(x, y)
#define DPRINT3(x, y, z)
#endif

#define CHECK(exp)                      \
    if (!(exp))                         \
    {                                   \
        std::cout << "Ln "              \
                  << __LINE__           \
                  << ": CHECK failed: " \
                  << #exp               \
                  << std::endl;         \
        return {};                      \
    }
