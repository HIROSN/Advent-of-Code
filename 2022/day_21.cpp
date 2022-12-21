#include <main.h>
#include <skip.h>

#include <map>
#include <sstream>
#include <tuple>

#define OP 0
#define L 1
#define R 2

using Math = std::tuple<char, std::string, std::string>;
std::map<std::string, Math> maths;
std::map<std::string, int64_t> numbers;

bool read_input(std::ifstream &file);
int64_t yell(const std::string &monkey);

std::optional<uint64_t> Answer(std::ifstream &file)
{
    CHECK(read_input(file));
    return yell("root");
}

bool read_input(std::ifstream &file)
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
            numbers[monkey] = number;
        }
        else if (std::getline(ss, left, ' ') &&
                 std::getline(ss, operation, ' ') &&
                 std::getline(ss, right, ' ') &&
                 operation.size())
        {
            maths[monkey] = {operation[0], left, right};
        }
        else
        {
            return false;
        }
    }
    return true;
}

int64_t yell(const std::string &monkey)
{
    if (numbers.find(monkey) != numbers.end())
        return numbers[monkey];

    const Math &math = maths[monkey];
    const std::string &left = std::get<L>(math);
    const std::string &right = std::get<R>(math);

    switch (std::get<OP>(math))
    {
    case '+':
        return yell(left) + yell(right);
    case '-':
        return yell(left) - yell(right);
    case '*':
        return yell(left) * yell(right);
    case '/':
        return yell(left) / yell(right);
    }

    return 0;
}
