// #include <debug_print.h>
#include <main.h>
#include <skip.h>

#include <map>
#include <queue>
#include <sstream>
#include <utility>
#include <vector>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    std::string line;
    std::getline(file, line);
    std::stringstream ss(line);
    std::priority_queue<std::pair<int, std::string>> patterns_queue;
    std::vector<std::string> towel_patterns;
    using Cache = std::map<std::string, uint64_t>;
    Cache possible_cache;
    Cache impossible_cache;
    uint64_t possible_designs = 0;

    while (ss.good())
    {
        std::string pattern;
        ss >> pattern;
        pattern = pattern.substr(0, pattern.find(','));
        patterns_queue.push({pattern.size(), pattern});
    }

    while (!patterns_queue.empty())
    {
        towel_patterns.push_back(patterns_queue.top().second);
        patterns_queue.pop();
    }

    while (std::getline(file, line))
    {
        if (line.empty())
            continue;

        struct Design
        {
            std::string towels;
            std::vector<std::string> *towel_patterns;
            Cache *possible;
            Cache *impossible;

            Design(std::string t, std::vector<std::string> *tp,
                   Cache *pc, Cache *ic) : towels(t), towel_patterns(tp),
                                           possible(pc), impossible(ic) {}
            uint64_t review(const Design &design)
            {
                if (possible->find(design.towels) != possible->end())
                    return (*possible)[design.towels];

                if (impossible->find((design.towels)) != impossible->end())
                    return 0;

                uint64_t number_of_designs = 0;

                for (const auto &pattern : *towel_patterns)
                {
                    if (design.towels == pattern)
                    {
                        number_of_designs++;
                        continue;
                    }

                    if (design.towels.find(pattern) == 0)
                    {
                        auto new_design = design;
                        new_design.towels = design.towels.substr(pattern.size());
                        number_of_designs += new_design.review(new_design);
                    }
                }

                if (number_of_designs)
                    (*possible)[towels] = number_of_designs;
                else
                    (*impossible)[towels]++;

                return number_of_designs;
            }
        };

        Design design(line, &towel_patterns,
                      &possible_cache, &impossible_cache);

        uint64_t number_of_designs = design.review(design);
        if (number_of_designs)
            possible_designs += number_of_designs;

        DPRINTX_ENDL(number_of_designs
                     << ": " << design.towels
                     << (number_of_designs ? "" : " is impossible"));
    }

    return possible_designs;
}
