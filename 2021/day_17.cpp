// #include "debug_print.h"
#include "main.h"

#include <algorithm>
#include <map>
#include <sstream>
#include <utility>

int target_sx, target_sy, target_ex, target_ey;
std::map<std::pair<int, int>, char> diagram;

bool read_input(std::ifstream &file, int &highest_y);
int launch(const int vx, const int vy);

uint64_t Answer(std::ifstream &file)
{
    int highest_y = 0;

    if (!read_input(file, highest_y))
    {
        return highest_y;
    }

    for (int vx = 1; vx <= target_sx; vx++)
    {
        for (int vy = 1; vy <= -target_sy; vy++)
        {
            highest_y = std::max(highest_y, launch(vx, vy));
        }
    }

    return highest_y;
}

int launch(const int vx, const int vy)
{
    int velocity_x = vx;
    int velocity_y = vy;
    int highest_y = 0;
    int probe_x = 0;
    int probe_y = 0;
    bool reached_target = false;

    for (int step = 1; !reached_target && probe_y >= target_sy; step++)
    {
        probe_x += velocity_x;
        probe_y += velocity_y;

        if (probe_y > highest_y)
        {
            highest_y = probe_y;
        }

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
#ifdef DPRINT_ON
        std::cout << highest_y << " ";
        DPRINT2(vx, vy);
        DPRINT_ENDL();
#endif
        return highest_y;
    }
    else
    {
        return 0;
    }
}

bool read_input(std::ifstream &file, int &highest_y)
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
            highest_y = launch(velocity_x, velocity_y);
            DPRINT2(diagram, '.');
            return false;
        }
    }

    return ok;
}
