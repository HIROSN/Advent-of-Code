#include <main.h>
#include <skip.h>

#include <sstream>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    auto getline = [&]() -> uint64_t
    {
        std::string line;
        std::getline(file, line);
        std::stringstream ss(line);
        skip<std::string>(ss);
        uint64_t number;
        std::stringstream ss_out;
        while (ss >> number)
            ss_out << number;
        ss_out >> number;
        return number;
    };

    uint64_t time = getline();
    uint64_t distance = getline();
    uint64_t half_time = time / 2;
    uint64_t ways_to_win = 0;

    auto is_win = [&](uint64_t msec) -> bool
    {
        uint64_t mm = time - msec;
        if (msec * mm > distance)
        {
            ways_to_win++;
            return true;
        }
        return false;
    };

    for (uint64_t msec = half_time; msec > 0; msec--)
        if (!is_win(msec))
            break;

    for (uint64_t msec = half_time + 1; msec < time; msec++)
        if (!is_win(msec))
            break;

    return ways_to_win;
}
