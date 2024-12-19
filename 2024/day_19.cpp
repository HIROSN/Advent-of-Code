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
    using Cache = std::map<std::string, int>;
    Cache possible_cache;
    Cache impossible_cache;
    int possible_designs = 0;

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
            bool review(const Design &design)
            {
                if (possible->find(design.towels) != possible->end())
                    return true;

                if (impossible->find((design.towels)) != impossible->end())
                    return false;

                bool is_design_possible = false;

                for (const auto &pattern : *towel_patterns)
                {
                    if (design.towels == pattern)
                        return true;

                    if (design.towels.find(pattern) == 0)
                    {
                        auto new_design = design;
                        new_design.towels = design.towels.substr(pattern.size());

                        if (new_design.review(new_design))
                        {
                            is_design_possible = true;
                            break;
                        }
                    }
                }

                if (is_design_possible)
                    (*possible)[towels]++;
                else
                    (*impossible)[towels]++;

                return is_design_possible;
            }
        };

        Design design(line, &towel_patterns,
                      &possible_cache, &impossible_cache);
        DPRINTX(possible_designs << ": " << design.towels);

        auto possible = design.review(design);
        if (possible)
            possible_designs++;

        DPRINTX_ENDL((possible ? "" : " is impossible"));
    }

    return possible_designs;
}
