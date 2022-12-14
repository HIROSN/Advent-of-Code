#include "main.h"

#include <algorithm>
#include <cmath>
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

using HistogramType = std::unordered_map<int, int>;

struct ScannerPair
{
    int first = 0;
    int orientation = 0;
    int second = 0;
};

struct ScannerInfo
{
    bool know_position = false;
    int orientation = 0;
    int relative_to_0[3] = {};
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

bool get_next(ScannerPair &pair);
bool find_overlap(const ScannerPair &pair);
bool overlaps(int a, int s0, int s1, int o0, int o1, int *r);
int manhattan_distance();
int axis(int a);
int rotate(int a, int o);
bool read_scanners(std::ifstream &file);

uint64_t Answer(std::ifstream &file)
{
    CHECK(read_scanners(file));
    ScannerPair scanner_pair;

    while (get_next(scanner_pair))
    {
        find_overlap(scanner_pair);
    }

    return manhattan_distance();
}

bool get_next(ScannerPair &pair)
{
    bool go_next = false;

    for (int s0 = 0; !go_next && s0 < number_of_scanners; s0++)
    {
        if (scanner_info[s0].know_position)
        {
            for (int s1 = 0; s1 < number_of_scanners; s1++)
            {
                if (!scanner_info[s0].done[s1])
                {
                    pair = {s0, scanner_info[s0].orientation, s1};
                    go_next = true;
                    break;
                }
            }
        }
    }

    return go_next;
}

bool find_overlap(const ScannerPair &pair)
{
    int s0 = pair.first;
    int o0 = pair.orientation;
    int s1 = pair.second;
    scanner_info[s0].done[s1] = true;
    scanner_info[s1].done[s0] = true;

    for (int o1 = 0; o1 < max_orientations; o1++)
    {
        int relative[3] = {};

        if (overlaps(X, s0, s1, o0, o1, relative) &&
            overlaps(Y, s0, s1, o0, o1, relative) &&
            overlaps(Z, s0, s1, o0, o1, relative))
        {
            scanner_info[s1].know_position = true;
            scanner_info[s1].orientation = o1;

            for (int a = axis(X); a <= axis(Z); a++)
            {
                scanner_info[s1].relative_to_0[a] =
                    scanner_info[s0].relative_to_0[a] + relative[a];
            }

            return true;
        }
    }

    return false;
}

bool overlaps(int a, int s0, int s1, int o0, int o1, int *r)
{
    std::unordered_map<int, int> shift;
    int double_count = 0;

    for (auto h0 : histogram[s0][rotate(a, o0)])
    {
        for (auto h1 : histogram[s1][rotate(a, o1)])
        {
            int s = h0.first - h1.first;
            shift[s] += h0.second + h1.second;

            if (shift[s] > double_count)
            {
                double_count = shift[s];
                r[axis(a)] = s;
            }
        }
    }

    return double_count >= 2 * at_least;
}

int manhattan_distance()
{
    int largest_manhattan_distance = 0;

    for (int s0 = 0; s0 < number_of_scanners; s0++)
    {
        for (int s1 = 0; s1 < number_of_scanners; s1++)
        {
            int *relative_0 = scanner_info[s0].relative_to_0;
            int *relative_1 = scanner_info[s1].relative_to_0;
            int md = std::abs(relative_0[axis(X)] - relative_1[axis(X)]) +
                     std::abs(relative_0[axis(Y)] - relative_1[axis(Y)]) +
                     std::abs(relative_0[axis(Z)] - relative_1[axis(Z)]);

            if (md > largest_manhattan_distance)
            {
                largest_manhattan_distance = md;
            }
        }
    }

    return largest_manhattan_distance;
}

int axis(int a)
{
    return a > 0 ? a - 1 : 2 - a;
}

int rotate(int a, int o)
{
    return axis(orientations[o][axis(a)]);
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
