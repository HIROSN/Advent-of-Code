// #include <debug_print.h>
#include <main.h>

#include <sstream>
#include <utility>
#include <vector>

enum class Facing : int
{
    Right = 0,
    Down = 1,
    Left = 2,
    Up = 3,
};

using Position = std::pair<std::pair<int, int>, Facing>;

constexpr char arrow[] = {'>', 'v', '<', '^'};
constexpr int arrow_direction[] = {+1, +1, -1, -1};
constexpr int arrow_size = sizeof(arrow) / sizeof(arrow[0]);

std::vector<std::string> board;
int size_x = 0;
int size_y = 0;

int &position_x(Position &position);
int &position_y(Position &position);
Facing &position_facing(Position &position);
int index(const Facing &facing);
Facing facing(int index);
void move(Position &current, int number, std::vector<Position> &trace);
bool move_one(Position &current);
void turn(Position &current, char letter);

#ifdef DPRINT_ON
bool debug_print(std::vector<Position> &trace);
#endif

std::optional<uint64_t> Answer(std::ifstream &file)
{
    std::string path, line;

    while (std::getline(file, line))
    {
        if (!line.empty())
        {
            board.push_back(line);

            if (line.size() > size_x)
                size_x = line.size();
        }
        else
        {
            std::getline(file, path);
        }
    }
    size_y = board.size();

    for (auto &row : board)
    {
        if (row.size() < size_x)
            row += std::string(size_x - row.size(), ' ');
    }
    DPRINT(board);

    Position current;
    position_y(current) = 0;
    position_facing(current) = Facing::Right;

    for (int x = 0; x < size_x; x++)
    {
        if (board[position_y(current)][x] != ' ')
        {
            position_x(current) = x;
            break;
        }
    }

    std::vector<Position> position_trace;
    std::stringstream ss(path);

    while (ss.good())
    {
        int number = 0;
        char letter = 0;
        if (ss.peek() <= '9')
        {
            if (ss >> number)
                move(current, number, position_trace);
        }
        else
        {
            if (ss >> letter)
                turn(current, letter);
        }
    }

#ifdef DPRINT_ON
    position_trace.push_back(current);
    if (!debug_print(position_trace))
        return {};

    DPRINT2(position_x(current), position_y(current));
    DPRINT_ENDL();
#endif

    return (position_y(current) + 1) * 1000 +
           (position_x(current) + 1) * 4 +
           index(position_facing(current));
}

int &position_x(Position &position)
{
    return position.first.first;
}

int &position_y(Position &position)
{
    return position.first.second;
}

Facing &position_facing(Position &position)
{
    return position.second;
}

int index(const Facing &facing)
{
    return static_cast<int>(facing);
}

Facing facing(int i)
{
    return static_cast<Facing>(i);
}

void move(Position &current, int number, std::vector<Position> &trace)
{
    for (int i = 0; i < number; i++)
    {
        trace.push_back(current);

        if (!move_one(current))
        {
            trace.pop_back();
            break;
        }
    }
}

bool move_one(Position &current)
{
    std::vector<char *> path;
    int index_current = 0;
    int direction = arrow_direction[index(position_facing(current))];

    switch (position_facing(current))
    {
    case Facing::Right:
    case Facing::Left:
        for (int x = 0; x < size_x; x++)
            path.push_back(&board[position_y(current)][x]);
        index_current = position_x(current);
        break;
    case Facing::Down:
    case Facing::Up:
        for (int y = 0; y < size_y; y++)
            path.push_back(&board[y][position_x(current)]);
        index_current = position_y(current);
        break;
    }

#ifdef DPRINT_ON
    // for (const auto &ptr : path)
    //     std::cout << *ptr;
    // std::cout << std::endl;
#endif

    int tiles_start = 0;
    for (; tiles_start < path.size(); tiles_start++)
    {
        if (*path[tiles_start] != ' ')
            break;
    }
    int tiles_offset = index_current - tiles_start;

    int tiles_size = path.size();
    for (; tiles_size >= 0; tiles_size--)
    {
        if (*path[tiles_size - 1] != ' ')
            break;
    }
    tiles_size -= tiles_start;

    tiles_offset += direction;
    if (tiles_offset < 0)
        tiles_offset += tiles_size;
    tiles_offset %= tiles_size;

#ifdef DPRINT_ON
    // std::cout << "start " << tiles_start << ", offset " << tiles_offset
    //           << ", size " << tiles_size << std::endl;
#endif

    if (*path[tiles_start + tiles_offset] == '#')
        return false;

    switch (position_facing(current))
    {
    case Facing::Right:
    case Facing::Left:
        position_x(current) = tiles_start + tiles_offset;
        break;
    case Facing::Down:
    case Facing::Up:
        position_y(current) = tiles_start + tiles_offset;
        break;
    }

    return true;
}

void turn(Position &current, char letter)
{
    int index_facing = index(position_facing(current));

    switch (letter)
    {
    case 'R':
        index_facing++;
        break;
    case 'L':
        index_facing--;
        break;
    }

    if (index_facing < 0)
        index_facing += arrow_size;

    position_facing(current) = facing(index_facing % arrow_size);
}

#ifdef DPRINT_ON
bool debug_print(std::vector<Position> &trace)
{
    bool is_valid = true;

    for (auto &p : trace)
    {
        auto &tile = board[position_y(p)][position_x(p)];
        if (tile == ' ' || tile == '#')
            is_valid = false;
        tile = arrow[index(position_facing(p))];
    }
    DPRINT(board);

    return is_valid;
}
#endif
