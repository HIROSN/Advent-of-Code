#include "main.h"

#include <algorithm>
#include <queue>
#include <vector>

struct Step
{
    bool on;
    int x1;
    int x2;
    int y1;
    int y2;
    int z1;
    int z2;

    int64_t cubes() const;
    bool operator&&(const Step &right) const;
    Step operator&(const Step &right) const;
};

using Steps = std::vector<Step>;

Steps read_steps(std::ifstream &file);
uint64_t cubes_on(const Steps &updated_steps);

uint64_t Answer(std::ifstream &file)
{
    auto steps = read_steps(file);
    Steps updated_steps;

    for (auto &step : steps)
    {
        if (updated_steps.empty())
        {
            updated_steps.push_back(step);
            continue;
        }

        Steps new_steps;

        if (step.on)
        {
            new_steps.push_back(step);
        }

        for (auto &prev_step : updated_steps)
        {
            if (step && prev_step)
            {
                auto overlap = step & prev_step;
                overlap.on = !prev_step.on;
                new_steps.push_back(overlap);
            }
        }

        updated_steps.insert(
            updated_steps.end(), new_steps.begin(), new_steps.end());
    }

    return cubes_on(updated_steps);
}

int64_t Step::cubes() const
{
    int64_t c = 1;
    c *= x2 - x1 + 1;
    c *= y2 - y1 + 1;
    c *= z2 - z1 + 1;
    return c;
}

bool Step::operator&&(const Step &r) const
{
    return (x1 <= r.x1 && r.x1 <= x2 || r.x1 <= x1 && x1 <= r.x2) &&
           (y1 <= r.y1 && r.y1 <= y2 || r.y1 <= y1 && y1 <= r.y2) &&
           (z1 <= r.z1 && r.z1 <= z2 || r.z1 <= z1 && z1 <= r.z2);
}

Step Step::operator&(const Step &right) const
{
    Step step;
    step.on = false;
    step.x1 = std::max(x1, right.x1);
    step.x2 = std::min(x2, right.x2);
    step.y1 = std::max(y1, right.y1);
    step.y2 = std::min(y2, right.y2);
    step.z1 = std::max(z1, right.z1);
    step.z2 = std::min(z2, right.z2);
    return step;
}

Steps read_steps(std::ifstream &file)
{
    Steps steps;
    std::string line;

    while (std::getline(file, line))
    {
        char turn[3 + 1] = {};
        int x1, x2, y1, y2, z1, z2;

        if (sscanf(line.c_str(),
                   "%s x=%d..%d,y=%d..%d,z=%d..%d",
                   turn, &x1, &x2, &y1, &y2, &z1, &z2) == 7)
        {
            static const std::string on("on");
            Step step{turn == on, x1, x2, y1, y2, z1, z2};
            steps.push_back(step);
        }
    }

    return steps;
}

uint64_t cubes_on(const Steps &updated_steps)
{
    int64_t cubes = 0;
    std::priority_queue<int64_t> on;
    std::priority_queue<int64_t> off;

    for (auto &step : updated_steps)
    {
        if (step.on)
        {
            on.push(step.cubes());
        }
        else
        {
            off.push(step.cubes());
        }
    }

    while (true)
    {
        if (!on.empty())
        {
            cubes += on.top();
            on.pop();
        }

        if (!off.empty())
        {
            cubes -= off.top();
            off.pop();
        }

        if (on.empty() && off.empty())
        {
            break;
        }
    }

    return cubes;
}
