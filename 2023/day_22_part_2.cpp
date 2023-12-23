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

    Brick *fall()
    {
        auto *cube_ptr = can_fall();
        if (cube_ptr)
        {
            cube_ptr->brick_ptr->fall(*this);
            settled[cube_ptr->z] = false;
            return cube_ptr->brick_ptr;
        }
        return nullptr;
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

        DPRINTX_ENDL(*brick);
        bricks.push_back(std::move(brick));
    }

    while (floor.fall())
        continue;
    DPRINTX_ENDL(floor);

    auto clone_bricks = [](const std::vector<BrickPtr> &bricks)
        -> std::vector<BrickPtr>
    {
        std::vector<BrickPtr> bricks_clone;
        for (const auto &bp : bricks)
        {
            auto brick_clone = std::make_unique<Brick>();
            for (const auto &cp : bp->cubes)
            {
                auto cube_clone = std::make_unique<Cube>(
                    Cube{cp->x, cp->y, cp->z, brick_clone.get()});
                brick_clone->cubes.push_back(std::move(cube_clone));
            }
            bricks_clone.push_back(std::move(brick_clone));
        }
        return bricks_clone;
    };

    auto clone_floor = [](const FloorMap &floor,
                          std::vector<BrickPtr> &bricks_clone) -> FloorMap
    {
        FloorMap floor_clone;
        for (auto &brick_ptr : bricks_clone)
            for (auto &cube_ptr : brick_ptr->cubes)
                floor_clone.map[cube_ptr->z].push_back(cube_ptr.get());
        return floor_clone;
    };

    Number other_bricks_to_fall = 0;

    for (int i = 0; i < bricks.size(); i++)
    {
        auto bricks_clone = clone_bricks(bricks);
        auto floor_clone = clone_floor(floor, bricks_clone);
        std::map<Brick *, int> bricks_fallen;

        bricks_clone[i]->disintegrated(true);
        DPRINTX_ENDL("Disintegrated:" << *bricks_clone[i]);

        while (true)
        {
            auto *brick_fallen = floor_clone.fall();
            if (!brick_fallen)
                break;
            bricks_fallen[brick_fallen]++;

            DPRINTX_ENDL("Fallen:" << *brick_fallen);
            DPRINTX(floor_clone);
            DPRINT2(bricks_fallen.size(),
                    bricks_fallen[brick_fallen]);
            DPRINT_ENDL();
        }

        other_bricks_to_fall += bricks_fallen.size();
        bricks_clone[i]->disintegrated(false);
    }

    return other_bricks_to_fall;
}

std::ostream &operator<<(std::ostream &out, const Cube &cube)
{
#ifdef DPRINT_ON
    out << "{"
        << cube.x << "," << cube.y << "," << cube.z
        << "}";
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
    for (auto &cube_ptr : brick.cubes)
        out << *cube_ptr;
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
#endif
    return out;
}
