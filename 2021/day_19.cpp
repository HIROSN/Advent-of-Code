// #include "debug_print.h"
#include "main.h"

#include <algorithm>
#include <map>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

#define X 1
#define Y 2
#define Z 3

const int max_scanners = 31;
const int max_beacons = 26;
const int max_axes = 6;
const int max_orientations = 24;
const int at_least = 12;

using HistogramType = std::map<int, int>;
using ScannerPair = std::pair<int, int>;
using BeaconMap = std::map<std::tuple<int, int, int>, int>;

struct ScannerInfo
{
    bool know_position = false;
    int orientation = 0;
    int shift[3] = {};
    bool done[max_scanners] = {};
};

int number_of_scanners = 0;
int number_of_beacons[max_scanners] = {};
int beacons[max_scanners][max_beacons][max_axes] = {};
HistogramType histogram[max_scanners][max_axes];
ScannerInfo scanner_info[max_scanners] = {};

const int orientations[max_orientations][3] = {
    {X, Y, Z},
    {X, Z, -Y},
    {X, -Y, -Z},
    {X, -Z, Y},
    {-X, Y, -Z},
    {-X, -Z, -Y},
    {-X, -Y, Z},
    {-X, Z, Y},
    {Y, X, -Z},
    {Y, -Z, -X},
    {Y, -X, Z},
    {Y, Z, X},
    {-Y, X, Z},
    {-Y, Z, -X},
    {-Y, -X, -Z},
    {-Y, -Z, X},
    {Z, X, Y},
    {Z, Y, -X},
    {Z, -X, -Y},
    {Z, -Y, X},
    {-Z, X, -Y},
    {-Z, -Y, -X},
    {-Z, -X, Y},
    {-Z, Y, X},
};

bool get_next(ScannerPair &pair, int &o0);
bool find(const ScannerPair &pair, int o0);
bool overlaps(int s0, int s1, int o0, int o1, int *r, int a);
bool valid(int s0, int s1, int o0, int o1, int *r);
BeaconMap full_map_of_beacons();
int axis(int a);
int axis(int a, int o);
bool read_scanners(std::ifstream &file);

uint64_t Answer(std::ifstream &file)
{
    CHECK(read_scanners(file));
    ScannerPair scanner_pair;
    int orientation = 0;

    while (get_next(scanner_pair, orientation))
    {
        find(scanner_pair, orientation);
    }

    return full_map_of_beacons().size();
}

bool get_next(ScannerPair &pair, int &o0)
{
    bool go_next = false;
    int s0 = 0;
    int s1 = 0;

    for (s0 = 0; !go_next && s0 < number_of_scanners; s0++)
    {
        if (scanner_info[s0].know_position)
        {
            for (s1 = 0; s1 < number_of_scanners; s1++)
            {
                if (!scanner_info[s0].done[s1])
                {
                    pair = {s0, s1};
                    o0 = scanner_info[s0].orientation;
                    go_next = true;
                    break;
                }
            }
        }
    }

    return go_next;
}

bool find(const ScannerPair &pair, int o0)
{
    int s0 = pair.first;
    int s1 = pair.second;
    scanner_info[s0].done[s1] = true;
    scanner_info[s1].done[s0] = true;

    for (int o1 = 0; o1 < max_orientations; o1++)
    {
        int r[3] = {};

        if (overlaps(s0, s1, o0, o1, r, X) &&
            overlaps(s0, s1, o0, o1, r, Y) &&
            overlaps(s0, s1, o0, o1, r, Z))
        {
            if (valid(s0, s1, o0, o1, r))
            {
                for (int i = 0; i < 3; i++)
                {
                    scanner_info[s1].shift[i] =
                        scanner_info[s0].shift[i] + r[i];
                }
#ifdef DPRINT_ON
                std::cout << "Scanner " << s1 << " overlaps with scanner "
                          << s0 << ". Scanner " << s1 << " is at ";
                DPRINT3(scanner_info[s1].shift[0],
                        scanner_info[s1].shift[1],
                        scanner_info[s1].shift[2]);
                std::cout << " (relative to scanner 0)." << std::endl;
#endif
                scanner_info[s1].know_position = true;
                scanner_info[s1].orientation = o1;
                return true;
            }
        }
    }

    return false;
}

bool overlaps(int s0, int s1, int o0, int o1, int *r, int a)
{
    std::unordered_map<int, int> shift;
    int c = 0;

    for (auto h0 : histogram[s0][axis(a, o0)])
    {
        for (auto h1 : histogram[s1][axis(a, o1)])
        {
            int s = h0.first - h1.first;
            shift[s] += h0.second + h1.second;

            if (shift[s] > c)
            {
                c = shift[s];
                r[axis(a)] = s;
            }
        }
    }

    return c >= at_least * 2;
}

bool valid(int s0, int s1, int o0, int o1, int *r)
{
    int c = 0;

    for (int b0 = 0; b0 < number_of_beacons[s0]; b0++)
    {
        int b0_x = beacons[s0][b0][axis(X, o0)];
        int b0_y = beacons[s0][b0][axis(Y, o0)];
        int b0_z = beacons[s0][b0][axis(Z, o0)];

        HistogramType &h1_x = histogram[s1][axis(X, o1)];
        HistogramType &h1_y = histogram[s1][axis(Y, o1)];
        HistogramType &h1_z = histogram[s1][axis(Z, o1)];

        int b1_x = b0_x - r[axis(X)];
        int b1_y = b0_y - r[axis(Y)];
        int b1_z = b0_z - r[axis(Z)];

        if (h1_x.find(b1_x) != h1_x.end() && h1_x[b1_x] > 0 &&
            h1_y.find(b1_y) != h1_y.end() && h1_y[b1_y] > 0 &&
            h1_z.find(b1_z) != h1_z.end() && h1_z[b1_z] > 0)
        {
            c++;
        }
    }

    return c >= at_least;
}

BeaconMap full_map_of_beacons()
{
    BeaconMap beacon_map;

    for (int s = 0; s < number_of_scanners; s++)
    {
        const int &o = scanner_info[s].orientation;
        const int *r = scanner_info[s].shift;

        for (int b = 0; b < number_of_beacons[s]; b++)
        {
            auto beacon = std::make_tuple(
                beacons[s][b][axis(X, o)] + r[axis(X)],
                beacons[s][b][axis(Y, o)] + r[axis(Y)],
                beacons[s][b][axis(Z, o)] + r[axis(Z)]);

            beacon_map[beacon]++;
        }
    }

    return beacon_map;
}

int axis(int a)
{
    return a > 0 ? a - 1 : 2 - a;
}

int axis(int a, int o)
{
    return axis(orientations[o][a - 1]);
}

bool read_scanners(std::ifstream &file)
{
    std::string line;
    int index_scanner = 0;
    int index_beacon = 0;
    scanner_info[index_scanner].know_position = true;

    while (std::getline(file, line))
    {
        if (line.empty())
        {
            continue;
        }

        if (sscanf(line.c_str(), "--- scanner %d ---", &index_scanner) == 1)
        {
            if (index_scanner >= max_scanners)
            {
                return false;
            }

            if (index_scanner + 1 > number_of_scanners)
            {
                number_of_scanners = index_scanner + 1;
            }

            scanner_info[index_scanner].done[index_scanner] = true;
            index_beacon = 0;
            continue;
        }

        if (index_beacon >= max_beacons)
        {
            return false;
        }

        int bx = 0;
        int by = 0;
        int bz = 0;

        if (sscanf(line.c_str(), "%d,%d,%d", &bx, &by, &bz) == 3)
        {
            beacons[index_scanner][index_beacon][axis(X)] = bx;
            beacons[index_scanner][index_beacon][axis(Y)] = by;
            beacons[index_scanner][index_beacon][axis(Z)] = bz;
            beacons[index_scanner][index_beacon][axis(-X)] = -bx;
            beacons[index_scanner][index_beacon][axis(-Y)] = -by;
            beacons[index_scanner][index_beacon][axis(-Z)] = -bz;

            histogram[index_scanner][axis(X)][bx]++;
            histogram[index_scanner][axis(Y)][by]++;
            histogram[index_scanner][axis(Z)][bz]++;
            histogram[index_scanner][axis(-X)][-bx]++;
            histogram[index_scanner][axis(-Y)][-by]++;
            histogram[index_scanner][axis(-Z)][-bz]++;

            number_of_beacons[index_scanner] = ++index_beacon;
        }
    }

    return true;
}
