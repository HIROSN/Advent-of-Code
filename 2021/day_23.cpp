// #include "debug_print.h"
#include "main.h"

#include <algorithm>
#include <cmath>
#include <vector>

constexpr char open_space = '.';
constexpr char immediately_outside_room = ' ';
constexpr int hallway_size = 11;
constexpr int max_room_size = 4;

std::vector<std::string> diagram;
int room_size = 0;
int least_total_energy = 0;
uint64_t number_of_operations = 0;

struct SideRoom
{
    int outside;
    char amphipod;
    char space[max_room_size];

    bool is_full() const;
    bool is_open() const;
    int enter(char amphipod);
    int leave(char &amphipod);
};

struct Situation
{
    int energy;
    char hallway[hallway_size];
    SideRoom rooms[4] = {{2, 'A'}, {4, 'B'}, {6, 'C'}, {8, 'D'}};

    bool is_organized() const;
    bool good() const;
};

void organize(const Situation &situation);
Situation enter(const Situation &situation, char amphipod, int from, int to);
Situation leave(const Situation &situation, char room, int from, int to);
bool is_amphipod(char space);
bool is_open(const std::string &hallway, int where, int where_to);
int energy_per_step(char amphipod);
Situation read_situation(std::ifstream &file);
void print_situation(const Situation &situation);

uint64_t Answer(std::ifstream &file)
{
    Situation situation = read_situation(file);
    print_situation(situation);
    organize(situation);
#ifdef DPRINT_ON
    std::cout << number_of_operations << " operations" << std::endl;
#endif
    return least_total_energy;
}

bool SideRoom::is_full() const
{
    for (int i = 0; i < room_size; i++)
    {
        if (space[i] != amphipod)
        {
            return false;
        }
    }
    return true;
}

bool SideRoom::is_open() const
{
    if (space[0] != open_space)
    {
        return false;
    }

    for (int i = 1; i < room_size; i++)
    {
        if (!(space[i] == open_space || space[i] == amphipod))
        {
            return false;
        }
    }

    return true;
}

int SideRoom::enter(char amphipod)
{
    for (int i = room_size - 1; i >= 0; i--)
    {
        if (space[i] == open_space)
        {
            space[i] = amphipod;
            return i + 1;
        }
    }

    return 0;
}

int SideRoom::leave(char &amphipod)
{
    for (int i = 0; i < room_size; i++)
    {
        if (space[i] != open_space)
        {
            amphipod = space[i];
            space[i] = open_space;
            return i + 1;
        }
    }

    return 0;
}

bool Situation::is_organized() const
{
    for (auto &room : rooms)
    {
        if (!room.is_full())
        {
            return false;
        }
    }
    return true;
}

bool Situation::good() const
{
    if (least_total_energy == 0)
    {
        return true;
    }

    if (energy >= least_total_energy)
    {
        return false;
    }

    int estimate = 0;

    for (int where = 0; where < hallway_size; where++)
    {
        if (is_amphipod(hallway[where]))
        {
            const auto &amphipod = hallway[where];
            const auto &where_to = rooms[amphipod - 'A'].outside;
            int steps = std::abs(where - where_to) + 1;
            estimate += steps * energy_per_step(amphipod);
        }
    }

    for (auto &room : rooms)
    {
        if (!room.is_full() && !room.is_open())
        {
            for (int i = 0; i < room_size; i++)
            {
                if (is_amphipod(room.space[i]))
                {
                    const auto &amphipod = room.space[i];
                    const auto &where = room.outside;
                    const auto &where_to = rooms[amphipod - 'A'].outside;
                    int steps = std::abs(where - where_to) + 2;
                    estimate += steps * energy_per_step(amphipod);
                }
            }
        }
    }

    return (energy + estimate) < least_total_energy;
}

void organize(const Situation &situation)
{
    if (situation.is_organized())
    {
        if (!least_total_energy || situation.energy < least_total_energy)
        {
            least_total_energy = situation.energy;
        }
        return;
    }

    if (!situation.good())
    {
        return;
    }

    for (int where = 0; where < hallway_size; where++)
    {
        if (is_amphipod(situation.hallway[where]))
        {
            const auto &amphipod = situation.hallway[where];
            const auto &where_to = situation.rooms[amphipod - 'A'].outside;

            if (is_open(situation.hallway, where, where_to) &&
                situation.rooms[amphipod - 'A'].is_open())
            {
                organize(enter(situation, amphipod, where, where_to));
            }
        }
    }

    for (auto &room : situation.rooms)
    {
        if (!room.is_full() && !room.is_open())
        {
            const int where = room.outside;

            for (int where_to = where; where_to < hallway_size; where_to++)
            {
                if (situation.hallway[where_to] == open_space &&
                    is_open(situation.hallway, where, where_to))
                {
                    organize(leave(situation, room.amphipod, where, where_to));
                }
            }

            for (int where_to = where; where_to >= 0; where_to--)
            {
                if (situation.hallway[where_to] == open_space &&
                    is_open(situation.hallway, where, where_to))
                {
                    organize(leave(situation, room.amphipod, where, where_to));
                }
            }
        }
    }

#ifdef DPRINT_ON
    number_of_operations++;
#endif
}

Situation enter(const Situation &situation, char amphipod, int from, int to)
{
    auto new_situation = situation;
    new_situation.hallway[from] = open_space;
    int steps = std::abs(from - to);
    steps += new_situation.rooms[amphipod - 'A'].enter(amphipod);
    new_situation.energy += steps * energy_per_step(amphipod);
    return new_situation;
}

Situation leave(const Situation &situation, char room, int from, int to)
{
    auto amphipod = open_space;
    auto new_situation = situation;
    int steps = std::abs(from - to);
    steps += new_situation.rooms[room - 'A'].leave(amphipod);
    new_situation.hallway[to] = amphipod;
    new_situation.energy += steps * energy_per_step(amphipod);
    return new_situation;
}

bool is_amphipod(char space)
{
    return energy_per_step(space) != 0;
}

bool is_open(const std::string &hallway, int where, int where_to)
{
    for (int i = std::min(where, where_to);
         i <= std::max(where, where_to);
         i++)
    {
        if (i != where && is_amphipod(hallway[i]))
        {
            return false;
        }
    }
    return true;
}

int energy_per_step(char amphipod)
{
    switch (amphipod)
    {
    case 'A':
        return 1;
    case 'B':
        return 10;
    case 'C':
        return 100;
    case 'D':
        return 1000;
    }
    return 0;
}

Situation read_situation(std::ifstream &file)
{
    Situation situation;
    std::string line;

    while (std::getline(file, line))
    {
        diagram.push_back(line);
        room_size += line.size() == 11 ? 1 : 0;
    }

    situation.energy = 0;

    for (int i = 0; i < hallway_size; i++)
    {
        situation.hallway[i] = diagram[1][i + 1];
    }

    for (auto &room : situation.rooms)
    {
        for (int i = 0; i < room_size; i++)
        {
            room.space[i] = diagram[i + 2][room.outside + 1];
        }

        situation.hallway[room.outside] = immediately_outside_room;
    }

    return situation;
}

void print_situation(const Situation &situation)
{
#ifdef DPRINT_ON
    diagram[1].replace(1, hallway_size, situation.hallway);

    for (auto &room : situation.rooms)
    {
        for (int i = 0; i < room_size; i++)
        {
            diagram[i + 2][room.outside + 1] = room.space[i];
        }
    }

    for (auto &line : diagram)
    {
        std::cout << line << std::endl;
    }
#endif
}
