#include <main.h>
#include <skip.h>

#include <sstream>
#include <unordered_map>
#include <vector>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    using WorkflowOrRatings = std::vector<std::string>;
    using Workflow = WorkflowOrRatings;
    using Rating = std::unordered_map<char, int>;

    Workflow workflows[26][26][27];
    std::vector<Rating> ratings;

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

    while (std::getline(file, line))
    {
        Rating parts_rating;

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

        ratings.push_back(parts_rating);
    }

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

    auto perform_step = [](Rating &rating, const Rule &rule) -> std::string
    {
        switch (rule.operation)
        {
        case '<':
            if (rating[rule.part] < rule.number)
                return rule.name;
            break;
        case '>':
            if (rating[rule.part] > rule.number)
                return rule.name;
            break;
        default:
            return rule.name;
        }
        return {};
    };

    int sum_of_rating_numbers_for_accepted_parts = 0;

    for (auto rating : ratings)
    {
        std::string name = "in";

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
        }

        if (name == "A")
        {
            for (auto it : rating)
                sum_of_rating_numbers_for_accepted_parts += it.second;
        }
    }

    return sum_of_rating_numbers_for_accepted_parts;
}
