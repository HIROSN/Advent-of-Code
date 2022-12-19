#include <main.h>

#include <map>
#include <tuple>

using Position = std::tuple<int, int, int>;

constexpr int grid_size = 21 + 1;
std::map<Position, bool> grid;
int surface_area = 0;

void update_surface_area(int x, int y, int z);

std::optional<uint64_t> Answer(std::ifstream &file)
{
    int x, y, z;
    char comma;

    while (file >> x >> comma >> y >> comma >> z)
        grid[{x, y, z}] = true;

    update_surface_area(-1, -1, -1);

    return surface_area;
}

void update_surface_area(int x, int y, int z)
{
    if (x >= -1 && x <= grid_size &&
        y >= -1 && y <= grid_size &&
        z >= -1 && z <= grid_size)
    {
        if (grid.find({x, y, z}) == grid.end())
        {
            grid[{x, y, z}] = false;
            update_surface_area(x - 1, y, z);
            update_surface_area(x + 1, y, z);
            update_surface_area(x, y - 1, z);
            update_surface_area(x, y + 1, z);
            update_surface_area(x, y, z - 1);
            update_surface_area(x, y, z + 1);
        }
        else if (grid[{x, y, z}])
        {
            surface_area++;
        }
    }
}
