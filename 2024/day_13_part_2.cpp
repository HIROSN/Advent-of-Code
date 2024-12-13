// #include <debug_print.h>
#include <main.h>
#include <skip.h>

#include <sstream>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    uint64_t fewest_tokens = 0;
    std::string line;

    while (std::getline(file, line))
    {
        int64_t ax, ay, bx, by, px, py;
        {
            std::stringstream ss(line);
            skip<std::string>(ss, 2);
            skip<char>(ss, 2);
            ss >> ax;
            skip<char>(ss, 3);
            ss >> ay;
        }

        std::getline(file, line);
        {
            std::stringstream ss(line);
            skip<std::string>(ss, 2);
            skip<char>(ss, 2);
            ss >> bx;
            skip<char>(ss, 3);
            ss >> by;
        }

        std::getline(file, line);
        {
            std::stringstream ss(line);
            skip<std::string>(ss, 1);
            skip<char>(ss, 2);
            ss >> px;
            skip<char>(ss, 3);
            ss >> py;
        }

        px += 10000000000000;
        py += 10000000000000;

        std::getline(file, line);
        DPRINT2(ax, ay);
        DPRINT2(bx, by);
        DPRINT2(px, py);

        int64_t dividend = ax * py - ay * px;
        int64_t divisor = ax * by - ay * bx;
        bool no_combination = (dividend % divisor) != 0;

        if (no_combination)
        {
            DPRINTX_ENDL(" Remainder (B) " << (dividend % divisor));
            continue;
        }

        const int64_t b_button = dividend / divisor;
        dividend = px - bx * b_button;
        divisor = ax;
        no_combination = (dividend % divisor) != 0;

        if (no_combination)
        {
            DPRINTX_ENDL(" Remainder (A) " << (dividend % divisor));
            continue;
        }

        const int64_t a_button = dividend / divisor;
        DPRINT2(a_button, b_button);

        if (a_button < 0 || b_button < 0)
        {
            DPRINTX_ENDL(" Out of range");
            continue;
        }

        uint64_t tokens = a_button * 3 + b_button;
        fewest_tokens += tokens;
        DPRINTX_ENDL(" Win " << tokens << " tokens");
    }

    return fewest_tokens;
}
