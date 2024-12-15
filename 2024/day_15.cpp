// #include <debug_print.h>
#include <main.h>

#include <queue>
#include <vector>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    struct Position
    {
        int x;
        int y;
    };

    struct Warehouse
    {
        std::vector<std::string> map;

        bool attempt(Position &position, char move)
        {
            char what = what_is_at(position);

            switch (what)
            {
            case '#':
                return false;
            case '.':
                return true;
            default:
                break;
            }

            auto next = position;
            auto offset = offsets[moves.find(move)];
            next.x += offset.x;
            next.y += offset.y;

            if (attempt(next, move))
            {
                what_is_at(position) = '.';
                position.x += offset.x;
                position.y += offset.y;
                what_is_at(position) = what;
                return true;
            }

            return false;
        }

        char &what_is_at(const Position &p)
        {
            return map[p.y][p.x];
        }

        const std::string moves = "^v<>";
        const std::vector<Position> offsets{{0, -1}, {0, 1}, {-1, 0}, {1, 0}};
    };

    Position robot;
    Warehouse warehouse;
    std::queue<char> moves;
    std::string line;
    char move;

    while (std::getline(file, line))
    {
        if (line.empty())
            break;

        int at = line.find('@');
        if (at != std::string::npos)
        {
            robot.x = at;
            robot.y = warehouse.map.size();
        }
        warehouse.map.push_back(line);
    }

    while (file >> move)
        moves.push(move);

    const int size_x = warehouse.map[0].size();
    const int size_y = warehouse.map.size();
    DPRINT(warehouse.map);
    DPRINT2(robot.x, robot.y);
    DPRINT_ENDL();
    DPRINTX_ENDL(moves.size());

    while (!moves.empty())
    {
        warehouse.attempt(robot, moves.front());
        moves.pop();
    }

    DPRINT(warehouse.map);
    uint64_t sum_of_all_coordinates = 0;

    for (int y = 0; y < size_y; y++)
        for (int x = 0; x < size_x; x++)
            if (warehouse.what_is_at({x, y}) == 'O')
                sum_of_all_coordinates += 100 * y + x;

    return sum_of_all_coordinates;
}
