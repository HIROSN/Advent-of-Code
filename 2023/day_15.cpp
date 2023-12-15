#include <main.h>

#include <sstream>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    std::string line;
    std::getline(file, line);
    std::stringstream ss(line);

    auto HASH = [](const std::string &str) -> int
    {
        int current_value = 0;

        for (const auto &ASCII_code : str)
        {
            current_value += ASCII_code;
            current_value *= 17;
            current_value %= 256;
        }

        return current_value;
    };

    int sum_of_results = 0;

    while (ss.good())
    {
        std::string str;
        std::getline(ss, str, ',');
        sum_of_results += HASH(str);
    }

    return sum_of_results;
}
