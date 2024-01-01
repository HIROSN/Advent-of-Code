// #define PART_1
// #include <debug_print.h>
#include <main.h>

#include <sstream>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    double boundary_points = 0;
    double area_total = 0;

    struct Point
    {
        int64_t x = 0;
        int64_t y = 0;
    };

    Point digger, p0, p1, p2;
    char direction;
    int number, color;
    std::string line;

    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string str;
        ss >> direction >> number >> str;
#ifdef PART_1
        std::stringstream(str.substr(2, 6)) >> std::hex >> color;
#else
        std::stringstream(str.substr(2, 5)) >> std::hex >> number;
        direction = str[7];
#endif
        for (int i = 0; i < number; i++)
        {
            switch (direction)
            {
#ifdef PART_1
            case 'U':
                digger.y--;
                break;
            case 'D':
                digger.y++;
                break;
            case 'L':
                digger.x--;
                break;
            case 'R':
                digger.x++;
                break;
#else
            case '0':
                digger.x++;
                break;
            case '1':
                digger.y++;
                break;
            case '2':
                digger.x--;
                break;
            case '3':
                digger.y--;
                break;
#endif
            }

            if (boundary_points == 0)
                p0 = digger;
            else if (boundary_points == 1)
                p1 = digger;
            else
            {
                p2 = digger;
                double x1 = p1.x - p0.x;
                double y1 = p1.y - p0.y;
                double x2 = p2.x - p0.x;
                double y2 = p2.y - p0.y;
                area_total += (x1 * y2 - x2 * y1) / 2.0f;
                p1 = p2;
            }
            boundary_points++;
        }
    }

    DPRINT2(area_total, boundary_points);
    DPRINT_ENDL();

    return area_total + boundary_points / 2.0f + 1;
}
