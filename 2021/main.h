#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

std::vector<std::string> args;

uint64_t Answer(std::ifstream &file);

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        std::cout << "Usage: " << argv[0] << " input" << std::endl;
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file.is_open())
    {
        return 2;
    }

    for (int i = 0; i < argc; i++)
    {
        args.emplace_back(argv[i]);
    }

    std::cout << Answer(file) << std::endl;
    file.close();
    return 0;
}

#ifndef DPRINT_ON
#define DPRINT_ENDL()
#define DPRINT(x)
#define DPRINT2(x, y)
#define DPRINT3(x, y, z)
#endif

#define CHECK(exp) \
    if (!exp)      \
        return 0;
