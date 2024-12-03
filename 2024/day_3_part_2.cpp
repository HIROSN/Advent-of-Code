#include <main.h>

#include <regex>
#include <sstream>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    std::string line, corrupted_memory;

    while (std::getline(file, line))
    {
        corrupted_memory += line;
    }

    std::regex instruction(R"(mul\(([0-9]{1,3}),([0-9]{1,3})\)|do\(\)|don't\(\))");
    std::smatch match;
    uint64_t add_all_of_multiplications = 0;
    bool enabled = true;

    while (std::regex_search(corrupted_memory, match, instruction))
    {
        if (match[0] == "do()")
        {
            enabled = true;
        }
        else if (match[0] == "don't()")
        {
            enabled = false;
        }
        else if (enabled)
        {
            std::stringstream ss_x(match[1]);
            std::stringstream ss_y(match[2]);
            int x, y;
            ss_x >> x;
            ss_y >> y;
            add_all_of_multiplications += x * y;
        }
        corrupted_memory = match.suffix().str();
    }

    return add_all_of_multiplications;
}
