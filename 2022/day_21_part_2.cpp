// #include <debug_print.h>
#include <main.h>
#include <skip.h>

#include <climits>
#include <map>
#include <sstream>
#include <tuple>

#define OP 0
#define L 1
#define R 2

using Math = std::tuple<char, std::string, std::string>;

std::string root_left;
std::string root_right;

void reset_input(std::ifstream &file,
                 std::map<std::string, Math> &maths,
                 std::map<std::string, int64_t> &numbers);

int match(std::ifstream &file, int64_t humn);

std::optional<uint64_t> Answer(std::ifstream &file)
{
    int64_t humn_low = INT64_MIN / 1'000'000;
    int64_t humn_high = INT64_MAX / 1'000'000;

    int result_low = match(file, humn_low);
    int result_high = match(file, humn_high);

    while (true)
    {
        int64_t humn_middle = (humn_high + humn_low) / 2;
        int result_middle = match(file, humn_middle);

#ifdef DPRINT_ON
        std::cout << humn_low << "(" << result_low << ") "
                  << humn_middle << "(" << result_middle << ") "
                  << humn_high << "(" << result_high << ")"
                  << std::endl;
#endif
        if (result_middle == 0)
            return humn_middle;

        if (result_low == result_middle)
        {
            humn_low = humn_middle;
            result_low = result_middle;
        }
        else
        {
            humn_high = humn_middle;
            result_high = result_middle;
        }
    }

    return {};
}

void reset_input(std::ifstream &file,
                 std::map<std::string, Math> &maths,
                 std::map<std::string, int64_t> &numbers)
{
    static std::map<std::string, Math> cache_maths;
    static std::map<std::string, int64_t> cache_numbers;

    if (file.good())
    {
        std::string monkey, line, left, operation, right;
        int number;

        while (std::getline(file, monkey, ':'))
        {
            skip(file, ' ');
            std::getline(file, line);
            std::stringstream ss(line);

            if (ss.peek() < 'a' && ss >> number)
            {
                cache_numbers[monkey] = number;
            }
            else if (std::getline(ss, left, ' ') &&
                     std::getline(ss, operation, ' ') &&
                     std::getline(ss, right, ' ') &&
                     operation.size())
            {
                cache_maths[monkey] = {operation[0], left, right};
            }
            else
            {
                break;
            }
        }

        const auto root = cache_maths["root"];
        root_left = std::get<L>(root);
        root_right = std::get<R>(root);
    }

    maths = cache_maths;
    numbers = cache_numbers;
}

int match(std::ifstream &file, int64_t humn)
{
    std::map<std::string, Math> maths;
    std::map<std::string, int64_t> numbers;

    reset_input(file, maths, numbers);
    numbers["humn"] = humn;

    while (true)
    {
        auto maths_size = maths.size();
        auto it = maths.begin();

        while (true)
        {
            const Math &math = it->second;
            const std::string &left = std::get<L>(math);
            const std::string &right = std::get<R>(math);

            if (numbers.find(left) != numbers.end() &&
                numbers.find(right) != numbers.end())
            {
                int64_t number;
                switch (std::get<OP>(math))
                {
                case '+':
                    number = numbers[left] + numbers[right];
                    break;
                case '-':
                    number = numbers[left] - numbers[right];
                    break;
                case '*':
                    number = numbers[left] * numbers[right];
                    break;
                case '/':
                    number = numbers[left] / numbers[right];
                    break;
                }
                if (numbers.find(it->first) == numbers.end())
                    numbers[it->first] = number;
                it = maths.erase(it);
            }
            else
            {
                it++;
            }

            if (it == maths.end())
                break;
        }

        if (maths.empty() || maths.size() == maths_size)
            break;
    }

    if (numbers[root_left] > numbers[root_right])
        return -1;

    if (numbers[root_left] < numbers[root_right])
        return 1;

    return 0;
}
