// #include <debug_print.h>
#include <main.h>

#include <algorithm>
#include <map>
#include <queue>
#include <utility>
#include <vector>

int size_x, size_y;

std::optional<uint64_t> Answer(std::ifstream &file)
{
    using Platform = std::vector<std::string>;
    Platform platform;
    std::string line;

    while (std::getline(file, line))
        platform.push_back(line);

    DPRINT(platform);
    size_x = line.size();
    size_y = platform.size();

    enum class Tilt
    {
        North = 0,
        West,
        South,
        East,
        Size
    };

    struct RoundedRock
    {
        int x;
        int y;
        Tilt tilt;

        bool operator<(const RoundedRock &right) const
        {
            switch (tilt)
            {
            case Tilt::North:
                return y > right.y;
            case Tilt::West:
                return x > right.x;
            case Tilt::South:
                return y < right.y;
            case Tilt::East:
                return x < right.x;
            default:
                return false;
            }
        }
    };

    struct SpinCycle
    {
        Tilt tilt;
        std::priority_queue<RoundedRock> rounded_rocks;
        int total_load = 0;
        uint64_t hash = 0;

        void push(const RoundedRock &rock)
        {
            rounded_rocks.push(rock);
            hash += rock.x * rock.y;
        }

        void roll(Platform &p, SpinCycle &next)
        {
            while (!rounded_rocks.empty())
            {
                auto rock = rounded_rocks.top();
                int x = rock.x, y = rock.y, nx = rock.x, ny = rock.y;
                rounded_rocks.pop();

                switch (tilt)
                {
                case Tilt::North:
                    for (ny = y; ny > 0 && p[ny - 1][x] == '.'; ny--)
                        std::swap(p[ny - 1][x], p[ny][x]);
                    break;
                case Tilt::West:
                    for (nx = x; nx > 0 && p[y][nx - 1] == '.'; nx--)
                        std::swap(p[y][nx - 1], p[y][nx]);
                    break;
                case Tilt::South:
                    for (ny = y; ny < size_y - 1 && p[ny + 1][x] == '.'; ny++)
                        std::swap(p[ny + 1][x], p[ny][x]);
                    break;
                case Tilt::East:
                    for (nx = x; nx < size_x - 1 && p[y][nx + 1] == '.'; nx++)
                        std::swap(p[y][nx + 1], p[y][nx]);
                    break;
                default:
                    return;
                }

                next.push({nx, ny, next.tilt});
                total_load += size_y - ny;
            }
        }
    };

    const std::vector<Tilt> cycle_tilts{
        Tilt::North, Tilt::West, Tilt::South, Tilt::East};

    auto next_tilt = [&](Tilt tilt) -> Tilt
    {
        auto it = std::find(cycle_tilts.begin(), cycle_tilts.end(), tilt);

        if (++it == cycle_tilts.end())
            return *cycle_tilts.begin();
        else
            return *it;
    };

    auto do_one_cycle = [&](SpinCycle &current) -> int
    {
        int last_total_load = 0;

        for (int i = static_cast<int>(current.tilt);
             i < static_cast<int>(Tilt::Size);
             i++)
        {
            SpinCycle next{next_tilt(current.tilt)};
            current.roll(platform, next);
            last_total_load = current.total_load;
            current = next;
        }

        return last_total_load;
    };

    const Tilt &tilt = *cycle_tilts.begin();
    SpinCycle initial{tilt};
    SpinCycle cycle{next_tilt(tilt)};

    for (int y = 0; y < size_y; y++)
        for (int x = 0; x < size_x; x++)
            if (platform[y][x] == 'O')
                initial.rounded_rocks.push({x, y, initial.tilt});

    initial.roll(platform, cycle);
#ifdef DPRINT_ON
    DPRINT(platform);
    std::cout << initial.total_load << std::endl;
#endif

    struct CycleInfo
    {
        int count;
        int at_cycle;
        int cycle_size;
    };

    using Key = std::pair<int, uint64_t>;
    std::map<Key, CycleInfo> info_map;
    const int spin_cycle = 1000000000;
    int total_load_after_cycles = 0;

    for (int i = 0; i < spin_cycle; i++)
    {
        total_load_after_cycles = do_one_cycle(cycle);
        Key key{total_load_after_cycles, cycle.hash};

        if (info_map.find(key) != info_map.end())
        {
            auto &info = info_map[key];

            switch (info.count)
            {
            case 1:
                info.cycle_size = 0;
                break;
            case 2:
                info.cycle_size = i - info.at_cycle;
                break;
            default:
                if (i - info.at_cycle == info.cycle_size)
                {
                    while (i + info.cycle_size < spin_cycle)
                        i += info.cycle_size;
                }
                break;
            }

            info.count++;
            info.at_cycle = i;
        }
        else
        {
            info_map[key] = {1, i, 0};
        }
    }

    DPRINT(platform);
    return total_load_after_cycles;
}
