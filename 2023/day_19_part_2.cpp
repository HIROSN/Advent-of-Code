// #define PART_1
// #define DTIMER_ON
// #include <debug_print.h>
#include <main.h>
#include <skip.h>

#include <sstream>
#include <stack>
#include <vector>

constexpr int number_of_parts = 4;
const std::string parts = "xmas";

struct RatingRange
{
    int value[2][number_of_parts] = {};

    int &min(int part_or_index)
    {
        int i = parts.find(part_or_index);
        return value[0][i != std::string::npos ? i : part_or_index];
    }

    int &max(int part_or_index)
    {
        int i = parts.find(part_or_index);
        return value[1][i != std::string::npos ? i : part_or_index];
    }

    int &operator[](int part_or_index)
    {
        return min(part_or_index);
    }

    friend std::ostream &operator<<(
        std::ostream &out, RatingRange &rating);
};

std::optional<uint64_t> Answer(std::ifstream &file)
{
    using WorkflowOrRatings = std::vector<std::string>;
    using Workflow = WorkflowOrRatings;

    Workflow workflows[26][26][27];
    std::stack<RatingRange> ratings;

    auto workflow = [&](const std::string &name) -> Workflow &
    {
        return workflows[name[0] - 'a']
                        [name[1] - 'a']
                        [name.length() < 3 ? 0 : name[2] - 'a' + 1];
    };

    auto parse = [](const std::string &line) -> WorkflowOrRatings
    {
        auto open = line.find('{');
        auto close = line.find('}');
        std::stringstream ss(line.substr(open + 1, close - open - 1));
        WorkflowOrRatings list;
        std::string item;
        while (std::getline(ss, item, ','))
            list.push_back(item);
        return list;
    };

    std::string line;

    while (std::getline(file, line))
    {
        if (!line.empty())
        {
            auto name = line.substr(0, line.find('{'));
            workflow(name) = parse(line);
            continue;
        }
        break;
    }

#ifdef PART_1
    while (std::getline(file, line))
    {
        RatingRange parts_rating;

        for (const auto &str : parse(line))
        {
            std::stringstream ss(str);
            char part;
            int rating;
            ss >> part;
            skip<char>(ss);
            ss >> rating;
            parts_rating[part] = rating;
        }

        ratings.push(parts_rating);
    }
#else
    RatingRange parts_rating;
    for (const auto &part : parts)
    {
        parts_rating.min(part) = 1;
        parts_rating.max(part) = 4000;
    }
    ratings.push(parts_rating);
#endif

    struct Rule
    {
        char part = 0;
        char operation = 0;
        int number = 0;
        std::string name;
    };

    auto make_rule = [](const std::string &str) -> Rule
    {
        Rule rule;
        auto colon = str.find(':');

        if (colon == std::string::npos)
        {
            rule.name = str;
        }
        else
        {
            rule.part = str[0];
            rule.operation = str[1];
            std::stringstream(str.substr(2, colon - 2)) >> rule.number;
            rule.name = str.substr(colon + 1);
        }

        return rule;
    };

    auto perform_step = [&](RatingRange &rating, const Rule &rule) -> std::string
    {
        RatingRange left, right;
        if (rule.operation)
        {
            left = rating;
            right = rating;
        }

        switch (rule.operation)
        {
        case '<':
#ifndef PART_1
            if (rating.min(rule.part) < rule.number &&
                rating.max(rule.part) > rule.number)
            {
                left.max(rule.part) = rule.number - 1;
                right.min(rule.part) = rule.number;

                if (rating[rule.part] < rule.number)
                {
                    rating = left;
                    ratings.push(right);
                }
                else
                {
                    rating = right;
                    ratings.push(left);
                }
            }
#endif
            if (rating[rule.part] < rule.number)
                return rule.name;
            break;
        case '>':
#ifndef PART_1
            if (rating.min(rule.part) < rule.number &&
                rating.max(rule.part) > rule.number)
            {
                left.max(rule.part) = rule.number;
                right.min(rule.part) = rule.number + 1;

                if (rating[rule.part] > rule.number)
                {
                    rating = right;
                    ratings.push(left);
                }
                else
                {
                    rating = left;
                    ratings.push(right);
                }
            }
#endif
            if (rating[rule.part] > rule.number)
                return rule.name;
            break;
        default:
            return rule.name;
        }
        return {};
    };

    uint64_t answer = 0;

    while (!ratings.empty())
    {
        auto rating = ratings.top();
        ratings.pop();
        std::string name = "in";
        DPRINTX(rating << ": in");

        while (name.size() > 1)
        {
            std::string next_name;

            for (const auto &str : workflow(name))
            {
                next_name = perform_step(rating, make_rule(str));
                if (next_name.empty())
                    continue;
                break;
            }

            CHECK(next_name.size());
            name = next_name;
            DPRINTX(" -> " + name);
        }

        if (name == "A")
        {
#ifdef PART_1
            for (const auto &value : rating.value[0])
                answer += value;
#else
            uint64_t combinations = 1;
            for (int i = 0; i < number_of_parts; i++)
                combinations *= rating.max(i) - rating.min(i) + 1;
            answer += combinations;
            DPRINTX(" " << rating << " " << combinations);
#endif
        }
        DPRINT_ENDL();
    }

    return answer;
}

std::ostream &operator<<(std::ostream &out, RatingRange &rating)
{
#ifdef DPRINT_ON
    if (rating.max('x'))
    {
        out << "{x=" << rating['x'] << "~" << rating.max('x')
            << ",m=" << rating['m'] << "~" << rating.max('m')
            << ",a=" << rating['a'] << "~" << rating.max('a')
            << ",s=" << rating['s'] << "~" << rating.max('s')
            << "}";
    }
    else
    {
        out << "{x=" << rating['x']
            << ",m=" << rating['m']
            << ",a=" << rating['a']
            << ",s=" << rating['s']
            << "}";
    }
#endif
    return out;
}
