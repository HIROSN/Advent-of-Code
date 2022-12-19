#include <main.h>

constexpr int grid_size = 21 + 1;
bool grid[grid_size][grid_size][grid_size] = {};
int surface_area = 0;

void update_surface_area(int x, int y, int z);

std::optional<uint64_t> Answer(std::ifstream &file)
{
    int x, y, z;
    char comma;

    while (file >> x >> comma >> y >> comma >> z)
    {
        grid[x][y][z] = true;
        surface_area += 6;
    }

    for (x = 0; x < grid_size; x++)
    {
        for (y = 0; y < grid_size; y++)
        {
            for (z = 0; z < grid_size; z++)
            {
                if (grid[x][y][z])
                {
                    update_surface_area(x - 1, y, z);
                    update_surface_area(x + 1, y, z);
                    update_surface_area(x, y - 1, z);
                    update_surface_area(x, y + 1, z);
                    update_surface_area(x, y, z - 1);
                    update_surface_area(x, y, z + 1);
                }
            }
        }
    }

    return surface_area;
}

void update_surface_area(int x, int y, int z)
{
    if (x >= 0 && x < grid_size &&
        y >= 0 && y < grid_size &&
        z >= 0 && z < grid_size &&
        grid[x][y][z])
    {
        surface_area--;
    }
}
