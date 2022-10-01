// #include "debug_print.h"
#include "main.h"

#include <algorithm>
#include <map>
#include <sstream>
#include <utility>
#include <vector>

int target_sx, target_sy, target_ex, target_ey;
std::map<std::pair<int, int>, char> diagram;

bool read_input(std::ifstream &file);
bool launch(const int vx, const int vy);

uint64_t Answer(std::ifstream &file)
{
    std::vector<std::pair<int, int>> initial_velocity;

    if (!read_input(file))
    {
        return 0;
    }

    for (int vx = 1; vx <= target_ex; vx++)
    {
        for (int vy = target_sy; vy <= -target_sy; vy++)
        {
            if (launch(vx, vy))
            {
                auto velocity = std::make_pair(vx, vy);

                if (initial_velocity.empty() ||
                    std::find(initial_velocity.begin(),
                              initial_velocity.end(), velocity) ==
                        initial_velocity.end())
                {
                    initial_velocity.push_back(velocity);
                }
            }
        }
    }

    DPRINT_ENDL();
    return initial_velocity.size();
}

bool launch(const int vx, const int vy)
{
    int velocity_x = vx;
    int velocity_y = vy;
    int probe_x = 0;
    int probe_y = 0;
    bool reached_target = false;

    for (int step = 1; !reached_target && probe_y >= target_sy; step++)
    {
        probe_x += velocity_x;
        probe_y += velocity_y;

        if (probe_x >= target_sx && probe_x <= target_ex &&
            probe_y >= target_sy && probe_y <= target_ey)
        {
            reached_target = true;
        }

        if (velocity_x > 0)
        {
            velocity_x -= 1;
        }
        else if (velocity_x < 0)
        {
            velocity_x += 1;
        }

        velocity_y--;

#ifdef DPRINT_ON
        diagram[{probe_x, probe_y}] = '#';
#endif
    }

    if (reached_target)
    {
        DPRINT2(vx, vy);
        return true;
    }
    else
    {
        return false;
    }
}

bool read_input(std::ifstream &file)
{
    bool ok = false;
    std::string target, area, line_x, line_y;

    if (file >> target >> area >> line_x >> line_y)
    {
        if (sscanf(line_x.c_str(), "x=%d..%d,",
                   &target_sx, &target_ex) == 2 &&
            sscanf(line_y.c_str(), "y=%d..%d",
                   &target_sy, &target_ey) == 2)
        {
            ok = true;
        }
    }

    if (ok)
    {
#ifdef DPRINT_ON
        diagram[{0, 0}] = 'S';

        for (int x = target_sx; x <= target_ex; x++)
        {
            for (int y = target_sy; y <= target_ey; y++)
            {
                diagram[{x, y}] = 'T';
            }
        }
#endif
        if (args.size() == 4)
        {
            int velocity_x, velocity_y;
            std::stringstream(args[2]) >> velocity_x;
            std::stringstream(args[3]) >> velocity_y;
            launch(velocity_x, velocity_y);
            DPRINT_ENDL();
            DPRINT2(diagram, '.');
            return false;
        }
    }

    return ok;
}
