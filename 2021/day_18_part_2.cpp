// #include "debug_print.h"
#include "main.h"

#include <algorithm>
#include <memory>
#include <stack>
#include <vector>

struct SnailfishNumber;
struct RegularNumber;
struct SnailfishNumberPair;

using NumberType = std::unique_ptr<SnailfishNumber>;

struct SnailfishNumber
{
    struct Exploded
    {
        short add_to_left = 0;
        short add_to_right = 0;
    };

    bool explode();
    bool explode(const int nested, Exploded &exploded);
    bool explode(NumberType &pair, Exploded &exploded);
    void add_to_leftmost(short &value);
    void add_to_rightmost(short &value);
    bool split();
    bool split(NumberType &number);
    uint64_t magnitude();

    virtual bool is_regular_number() const;
    bool is_valid_pair();
    RegularNumber *as_number();
    SnailfishNumberPair *as_pair();

    friend std::ostream &operator<<(
        std::ostream &out, const NumberType &number);
};

struct SnailfishNumberPair : SnailfishNumber
{
    NumberType left;
    NumberType right;

    void add(NumberType number);
};

struct RegularNumber : SnailfishNumber
{
    short value = 0;

    bool is_regular_number() const override;
};

NumberType read_snailfish_number(const std::string line);
void print_number(std::ostream &out, SnailfishNumber &number);

uint64_t Answer(std::ifstream &file)
{
    std::vector<std::string> snailfish_numbers_list;
    std::string line;

    while (file >> line)
    {
        snailfish_numbers_list.push_back(line);
    }

    uint64_t largest_magnitude = 0;

    for (auto &x : snailfish_numbers_list)
    {
        for (auto &y : snailfish_numbers_list)
        {
            SnailfishNumberPair xy;
            xy.add(read_snailfish_number(x));
            xy.add(read_snailfish_number(y));
            largest_magnitude = std::max(largest_magnitude, xy.magnitude());

            SnailfishNumberPair yx;
            yx.add(read_snailfish_number(y));
            yx.add(read_snailfish_number(x));
            largest_magnitude = std::max(largest_magnitude, yx.magnitude());
        }
    }

    return largest_magnitude;
}

bool SnailfishNumber::explode()
{
    SnailfishNumber::Exploded exploded = {};
    return explode(1, exploded);
}

bool SnailfishNumber::explode(const int nested,
                              SnailfishNumber::Exploded &exploded)
{
    if (is_valid_pair())
    {
        if (nested == 4 && explode(as_pair()->left, exploded) ||
            as_pair()->left->explode(nested + 1, exploded))
        {
            as_pair()->right->add_to_leftmost(exploded.add_to_right);
            return true;
        }

        if (nested == 4 && explode(as_pair()->right, exploded) ||
            as_pair()->right->explode(nested + 1, exploded))
        {
            as_pair()->left->add_to_rightmost(exploded.add_to_left);
            return true;
        }
    }

    return false;
}

bool SnailfishNumber::explode(NumberType &pair,
                              SnailfishNumber::Exploded &exploded)
{
    if (!pair->is_regular_number())
    {
        auto &exploding_left = pair->as_pair()->left;
        auto &exploding_right = pair->as_pair()->right;

        if (exploding_left->is_regular_number())
        {
            exploded.add_to_left = exploding_left->as_number()->value;
        }

        if (exploding_right->is_regular_number())
        {
            exploded.add_to_right = exploding_right->as_number()->value;
        }

        NumberType zero = std::make_unique<RegularNumber>();
        pair.swap(zero);
        return true;
    }

    return false;
}

void SnailfishNumber::add_to_leftmost(short &value)
{
    if (value)
    {
        if (is_regular_number())
        {
            as_number()->value += value;
            value = 0;
        }
        else if (as_pair()->left)
        {
            as_pair()->left->add_to_leftmost(value);
        }
    }
}

void SnailfishNumber::add_to_rightmost(short &value)
{
    if (value)
    {
        if (is_regular_number())
        {
            as_number()->value += value;
            value = 0;
        }
        else if (as_pair()->right)
        {
            as_pair()->right->add_to_rightmost(value);
        }
    }
}

bool SnailfishNumber::split()
{
    if (is_valid_pair())
    {
        if (!as_pair()->left->is_regular_number() &&
                as_pair()->left->split() ||
            split(as_pair()->left))
        {
            return true;
        }

        return !as_pair()->right->is_regular_number() &&
                   as_pair()->right->split() ||
               split(as_pair()->right);
    }

    return false;
}

bool SnailfishNumber::split(NumberType &number)
{
    if (number->is_regular_number() &&
        number->as_number()->value >= 10)
    {
        auto split_value = number->as_number()->value;

        NumberType new_number_left = std::make_unique<RegularNumber>();
        new_number_left->as_number()->value = split_value / 2;

        NumberType new_number_right = std::make_unique<RegularNumber>();
        new_number_right->as_number()->value = split_value - (split_value / 2);

        NumberType new_pair = std::make_unique<SnailfishNumberPair>();
        new_pair->as_pair()->left = std::move(new_number_left);
        new_pair->as_pair()->right = std::move(new_number_right);

        number.swap(new_pair);
        return true;
    }

    return false;
}

uint64_t SnailfishNumber::magnitude()
{
    if (is_valid_pair())
    {
        return 3 * as_pair()->left->magnitude() +
               2 * as_pair()->right->magnitude();
    }

    if (is_regular_number())
    {
        return as_number()->value;
    }

    return 0;
}

void SnailfishNumberPair::add(NumberType number)
{
    if (!left)
    {
        left = std::move(number);
    }
    else if (!right)
    {
        right = std::move(number);
    }
    else
    {
        NumberType new_left = std::make_unique<SnailfishNumberPair>();
        NumberType new_right = std::move(number);
        left.swap(new_left);
        right.swap(new_right);
        left->as_pair()->left = std::move(new_left);
        left->as_pair()->right = std::move(new_right);
    }

    while (true)
    {
        if (explode())
        {
            continue;
        }

        if (split())
        {
            continue;
        }

        break;
    }
}

bool SnailfishNumber::is_regular_number() const
{
    return false;
}

bool SnailfishNumber::is_valid_pair()
{
    return !is_regular_number() && as_pair()->left && as_pair()->right;
}

RegularNumber *SnailfishNumber::as_number()
{
    return reinterpret_cast<RegularNumber *>(this);
}

SnailfishNumberPair *SnailfishNumber::as_pair()
{
    return reinterpret_cast<SnailfishNumberPair *>(this);
}

std::ostream &operator<<(std::ostream &out, const NumberType &number)
{
    if (number)
    {
        print_number(out, *number);
    }
    return out;
}

bool RegularNumber::is_regular_number() const
{
    return true;
}

NumberType read_snailfish_number(const std::string line)
{
    std::stack<NumberType> numbers;
    static const std::string non_numbers("[,]");

    for (auto &ch : line)
    {
        if (non_numbers.find(ch) == std::string::npos)
        {
            auto number = std::make_unique<RegularNumber>();
            number->value = ch - '0';
            numbers.push(std::move(number));
        }
        else if (ch == ']')
        {
            auto pair = std::make_unique<SnailfishNumberPair>();

            if (!numbers.empty())
            {
                pair->right = std::move(numbers.top());
                numbers.pop();

                if (!numbers.empty())
                {
                    pair->left = std::move(numbers.top());
                    numbers.pop();
                    numbers.push(std::move(pair));
                }
            }
        }
    }

    if (!numbers.empty())
    {
        return std::move(numbers.top());
        numbers.pop();
    }

    return {};
}

void print_number(std::ostream &out, SnailfishNumber &number)
{
#ifdef DPRINT_ON
    if (!number.is_regular_number())
    {
        out << "[";

        if (number.as_pair()->left)
        {
            if (number.as_pair()->left->is_regular_number())
            {
                out << number.as_pair()->left->as_number()->value;
            }
            else
            {
                print_number(out, *number.as_pair()->left);
            }
        }

        out << ",";

        if (number.as_pair()->right)
        {
            if (number.as_pair()->right->is_regular_number())
            {
                out << number.as_pair()->right->as_number()->value;
            }
            else
            {
                print_number(out, *number.as_pair()->right);
            }
        }

        out << "]";
    }
#endif
}
