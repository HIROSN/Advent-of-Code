// #include <debug_print.h>
#include <main.h>

#include <map>
#include <memory>
#include <sstream>
#include <vector>

using Number = unsigned int;
struct Cube;
struct Brick;
struct FloorMap;
using CubePtr = std::unique_ptr<Cube>;
using BrickPtr = std::unique_ptr<Brick>;

struct Cube
{
    Number x;
    Number y;
    Number z;
    Brick *brick_ptr;
    bool disintegrated = false;

    Cube make_fallen() const
    {
        auto copy = *this;
        copy.z--;
        return copy;
    }

    bool operator==(const Cube &right) const
    {
        return !disintegrated && x == right.x && y == right.y;
    }

    friend std::ostream &operator<<(std::ostream &out, const Cube &cube);
};

struct Brick
{
    std::vector<CubePtr> cubes;

    const Cube &low() const
    {
        return *cubes.begin()->get();
    }

    const Cube &high() const
    {
        return *cubes.rbegin()->get();
    }

    Number vertical() const
    {
        return high().z - low().z;
    }

    void disintegrated(bool disintegrated)
    {
        for (auto &cube : cubes)
            cube->disintegrated = disintegrated;
    }

    bool can_fall(FloorMap &floor_map);
    void fall(FloorMap &floor_map);

    friend std::ostream &operator<<(std::ostream &out, const Brick &brick);
};

struct FloorMap
{
    std::map<Number, std::vector<Cube *>> map;
    std::map<Number, bool> settled;

    bool can_add_cube(Cube *cube_ptr)
    {
        if (cube_ptr->z == 0)
            return false;
        if (map.find(cube_ptr->z) == map.end())
            return true;
        auto &cubes = map[cube_ptr->z];
        for (auto *has : cubes)
            if (*has == *cube_ptr)
                return false;
        return true;
    }

    void add_cube(Cube *cube_ptr)
    {
        DCHECK(can_add_cube(cube_ptr));
        map[cube_ptr->z].push_back(cube_ptr);
    }

    void remove_cube(Cube *cube_ptr)
    {
        auto &cubes = map[cube_ptr->z];
        auto it = cubes.begin();
        for (; it != cubes.end(); it++)
            if (**it == *cube_ptr)
                break;
        DCHECK(it != cubes.end());
        cubes.erase(it);
        if (cubes.empty())
            map.erase(cube_ptr->z);
    }

    bool can_add_block(Brick *brick_ptr)
    {
        for (auto &cube_ptr : brick_ptr->cubes)
            if (!can_add_cube(cube_ptr.get()))
                return false;
        return true;
    }

    void add_block(Brick *brick_ptr)
    {
        DCHECK(can_add_block(brick_ptr));
        for (auto &cube_ptr : brick_ptr->cubes)
            add_cube(cube_ptr.get());
    }

    void remove_block(Brick *brick_ptr)
    {
        for (auto &cube_ptr : brick_ptr->cubes)
            remove_cube(cube_ptr.get());
    }

    Cube *can_fall()
    {
        for (auto it : map)
        {
            if (settled[it.first])
                continue;
            for (auto &cube_ptr : it.second)
                if (cube_ptr->brick_ptr->can_fall(*this))
                    return cube_ptr;
            settled[it.first] = true;
        }
        return nullptr;
    }

    bool fall()
    {
        auto *cube_ptr = can_fall();
        if (cube_ptr)
        {
            cube_ptr->brick_ptr->fall(*this);
            settled[cube_ptr->z] = false;
            return true;
        }
        return false;
    }

    bool get_disintegrated(Brick *brick_ptr)
    {
        bool can_be_disintegrated = true;
        brick_ptr->disintegrated(true);

        Number above = brick_ptr->high().z + 1;
        if (map.find(above) != map.end())
        {
            for (auto *cube_ptr : map[above])
            {
                if (cube_ptr->brick_ptr->can_fall(*this))
                {
                    can_be_disintegrated = false;
                    break;
                }
            }
        }

        brick_ptr->disintegrated(false);
        return can_be_disintegrated;
    }

    friend std::ostream &operator<<(std::ostream &out, FloorMap &map);
};

bool Brick::can_fall(FloorMap &floor_map)
{
    for (auto &cube_ptr : cubes)
    {
        auto fallen = cube_ptr->make_fallen();
        if (!floor_map.can_add_cube(&fallen))
            return false;
        if (vertical())
            break;
    }
    return true;
}

void Brick::fall(FloorMap &floor_map)
{
    DCHECK(can_fall(floor_map));
    floor_map.remove_block(this);
    for (auto &cube_ptr : cubes)
        cube_ptr->z--;
    floor_map.add_block(this);
}

std::optional<uint64_t> Answer(std::ifstream &file)
{
    std::vector<BrickPtr> bricks;
    FloorMap floor;
    Number size_x = 0, size_y = 0, size_z = 0, cubes_size = 0;
    std::string line;

    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        auto brick = std::make_unique<Brick>();
        Cube c1, c2;
        char ch;

        ss >> c1.x >> ch >> c1.y >> ch >> c1.z;
        ss >> ch;
        ss >> c2.x >> ch >> c2.y >> ch >> c2.z;

        for (Number x = std::min(c1.x, c2.x);
             x < std::max(c1.x, c2.x) + 1; x++)
        {
            for (Number y = std::min(c1.y, c2.y);
                 y < std::max(c1.y, c2.y) + 1; y++)
            {
                for (Number z = std::min(c1.z, c2.z);
                     z < std::max(c1.z, c2.z) + 1; z++)
                {
                    auto cube = std::make_unique<Cube>(
                        Cube{x, y, z, brick.get()});
                    floor.map[cube->z].push_back(cube.get());
                    brick->cubes.push_back(std::move(cube));
                }
            }
        }

        bricks.push_back(std::move(brick));
    }

    for (const auto &brick_ptr : bricks)
    {
        DPRINTX(*brick_ptr);
        for (const auto &cube_ptr : brick_ptr->cubes)
        {
            DPRINTX(*cube_ptr);
            cubes_size++;
            size_x = std::max(size_x, cube_ptr->x);
            size_y = std::max(size_y, cube_ptr->y);
            size_z = std::max(size_z, cube_ptr->z + brick_ptr->vertical());
        }
        DPRINT_ENDL();
    }

    DPRINT3(size_x, size_y, size_z);
    DPRINT2(bricks.size(), cubes_size);
    DPRINT_ENDL();
    DPRINTX(floor);

    while (floor.fall())
        DPRINTX(floor);

    Number bricks_chosen_to_get_disintegrated = 0;
    for (auto &brick_ptr : bricks)
        if (floor.get_disintegrated(brick_ptr.get()))
            bricks_chosen_to_get_disintegrated++;

    return bricks_chosen_to_get_disintegrated;
}

std::ostream &operator<<(std::ostream &out, const Cube &cube)
{
#ifdef DPRINT_ON
    out << "{"
        << cube.x << "," << cube.y << "," << cube.z
        << "}"
        // << "->"
        // << *cube.brick_ptr
        ;
#endif
    return out;
}

std::ostream &operator<<(std::ostream &out, const Brick &brick)
{
#ifdef DPRINT_ON
    const auto &h = brick.high();
    const auto &l = brick.low();
    out << "["
        << h.x - l.x + 1 << "," << h.y - l.y + 1 << "," << h.z - l.z + 1
        << "]";
#endif
    return out;
}

std::ostream &operator<<(std::ostream &out, FloorMap &floor)
{
#ifdef DPRINT_ON
    std::map<Brick *, Number> bs;
    for (auto it = floor.map.rbegin(); it != floor.map.rend(); it++)
    {
        auto f = it->first;
        auto &s = floor.settled;
        out << (s.find(f) != s.end() && s[f] ? "#" : ".")
            << "F" << f << ":"
            << it->second.size() << ":";
        for (const auto &cp : it->second)
        {
            out << (cp->brick_ptr->can_fall(floor) ? " " : "R") << *cp;
            bs[cp->brick_ptr]++;
        }
        out << std::endl;
    }
    Number cs = 0;
    for (auto it : bs)
        cs += it.second;
    out << "{" << bs.size() << "," << cs << "}" << std::endl;
#endif
    return out;
}
