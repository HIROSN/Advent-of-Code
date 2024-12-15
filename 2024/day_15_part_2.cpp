// #include <debug_print.h>
#include <main.h>

#include <queue>
#include <vector>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    struct Position
    {
        int x = -1;
        int y = -1;

        bool empty()
        {
            return x < 0 || y < 0;
        }
    };

    struct Warehouse
    {
        std::vector<std::string> map;

        bool attempt(Position &position, char move,
                     bool do_not_commit = false)
        {
            if (position.empty())
                return true;

            char what = what_is_at(position), what_connected;
            Position connected;

            switch (what)
            {
            case '#':
                return false;

            case '.':
                return true;

            case '[':
            case ']':
                if (move == '<' || move == '>')
                    break;

                if (what == '[')
                    connected = {position.x + 1, position.y};
                else
                    connected = {position.x - 1, position.y};

                what_connected = what_is_at(connected);
                break;

            default:
                break;
            }

            auto next = next_position(position, move);
            auto next_connected = next_position(connected, move);

            if (attempt(next, move, do_not_commit) &&
                attempt(next_connected, move, do_not_commit))
            {
                if (!do_not_commit)
                {
                    do_move(what, position, move);
                    do_move(what_connected, connected, move);
                }
                return true;
            }

            return false;
        }

        void do_move(char what, Position &position, char move)
        {
            if (!position.empty())
            {
                what_is_at(position) = '.';
                auto offset = offsets[moves.find(move)];
                position.x += offset.x;
                position.y += offset.y;
                what_is_at(position) = what;
            }
        }

        Position next_position(const Position &position, char move)
        {
            auto next = position;
            auto offset = offsets[moves.find(move)];
            next.x += offset.x;
            next.y += offset.y;
            return next;
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

        std::string wide;

        for (const auto &ch : line)
            switch (ch)
            {
            case '#':
                wide += "##";
                break;
            case 'O':
                wide += "[]";
                break;
            case '.':
                wide += "..";
                break;
            case '@':
                wide += "@.";
                break;
            }

        int at = wide.find('@');

        if (at != std::string::npos)
        {
            robot.x = at;
            robot.y = warehouse.map.size();
        }

        warehouse.map.push_back(wide);
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
        char move = moves.front();
        DPRINTX_ENDL(move);

        if (move == '<' || move == '>')
            warehouse.attempt(robot, move);
        else if (warehouse.attempt(robot, move, true))
            warehouse.attempt(robot, move);

        moves.pop();
        DPRINT(warehouse.map);
    }

    uint64_t sum_of_all_coordinates = 0;

    for (int y = 0; y < size_y; y++)
        for (int x = 0; x < size_x; x++)
            if (warehouse.what_is_at({x, y}) == '[')
                sum_of_all_coordinates += 100 * y + x;

    return sum_of_all_coordinates;
}
