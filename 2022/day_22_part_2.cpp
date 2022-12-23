// #include <debug_print.h>
#include <main.h>

#include <algorithm>
#include <functional>
#include <map>
#include <sstream>
#include <utility>
#include <vector>

enum class Facing : int
{
    Right = 0,
    Down = 1,
    Left = 2,
    Up = 3,
    Size = 4
};

using Position = std::pair<std::pair<int, int>, Facing>;

constexpr char arrow[] = {'>', 'v', '<', '^'};
constexpr int arrow_direction[] = {+1, +1, -1, -1};
constexpr int arrow_size = sizeof(arrow) / sizeof(arrow[0]);

std::vector<std::string> board;
int size_x = 0;
int size_y = 0;
int face_size = 0;

int &position_x(Position &position);
int &position_y(Position &position);
Facing &position_facing(Position &position);
int index(const Facing &facing);
Facing facing(int index);
Facing facing_in(Facing facing_out);
void move(Position &current, int number, std::vector<Position> &trace);
bool move_one(Position &current);
void turn(Position &current, char letter);
std::optional<int> index_face(Position &position);
bool make_wrapping_rules();
bool apply_wrapping_rules(Position &current, Position &moved);

#ifdef DPRINT_ON
bool debug_print(std::vector<Position> &trace);
#endif

struct Face
{
    int index = 0;
    int left = 0;
    int top = 0;
    int right = 0;
    int bottom = 0;

    std::optional<int> connected_left;
    std::optional<int> connected_top;
    std::optional<int> connected_right;
    std::optional<int> connected_bottom;

    std::optional<int> corner_id[4];
    std::map<Facing, std::pair<int, int>> edges;

    Face(int id, int x, int y) : index(id),
                                 left(x),
                                 top(y),
                                 right(x + face_size - 1),
                                 bottom(y + face_size - 1) {}

    bool has(Position &position) const
    {
        const auto &x = position_x(position);
        const auto &y = position_y(position);
        return x >= left &&
               x < left + face_size &&
               y >= top &&
               y < top + face_size;
    }

    int number_of_corner_id() const
    {
        int number = 0;
        for (const auto &id : corner_id)
            if (id.has_value())
                number++;
        return number;
    }

    int block_x() const
    {
        return left / face_size;
    }

    int block_y() const
    {
        return top / face_size;
    }

    int to_edge_offset(Position &position) const
    {
        switch (position_facing(position))
        {
        case Facing::Right:
        case Facing::Left:
            return position_y(position) - top;
        case Facing::Down:
        case Facing::Up:
            return position_x(position) - left;
        }
        return 0;
    }

    void set_position(
        Position &position, int offset, bool reversed, Facing in)
    {
        switch (in)
        {
        case Facing::Right:
            position_x(position) = left;
            if (reversed)
                position_y(position) = bottom - offset;
            else
                position_y(position) = top + offset;
            break;
        case Facing::Down:
            position_y(position) = top;
            if (reversed)
                position_x(position) = right - offset;
            else
                position_x(position) = left + offset;
            break;
        case Facing::Left:
            position_x(position) = right;
            if (reversed)
                position_y(position) = bottom - offset;
            else
                position_y(position) = top + offset;
            break;
        case Facing::Up:
            position_y(position) = bottom;
            if (reversed)
                position_x(position) = right - offset;
            else
                position_x(position) = left + offset;
            break;
        }

        position_facing(position) = in;
    }
};

std::vector<Face> faces;

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
    CHECK(make_wrapping_rules());

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
    std::cout << "end ";
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

Facing facing_in(Facing facing_out)
{
    return facing((index(facing_out) + 2) % static_cast<int>(Facing::Size));
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
    auto moved = current;
    auto &moved_x = position_x(moved);
    auto &moved_y = position_y(moved);
    int direction = arrow_direction[index(position_facing(current))];

    switch (position_facing(current))
    {
    case Facing::Right:
    case Facing::Left:
        moved_x += direction;
        break;
    case Facing::Down:
    case Facing::Up:
        moved_y += direction;
        break;
    }

    auto face_into = index_face(moved);
    if (!face_into.has_value() && !apply_wrapping_rules(current, moved))
        return false;

    if (board[moved_y][moved_x] == '#')
        return false;

    current = std::move(moved);
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

std::optional<int> index_face(Position &position)
{
    for (const auto &face : faces)
    {
        if (face.has(position))
            return face.index;
    }
    return {};
}

bool make_wrapping_rules()
{
    face_size = std::min(size_x, size_y) / 3;
    int index = 0;
    for (int y = 0; y < size_y; y += face_size)
        for (int x = 0; x < size_x; x += face_size)
            if (board[y][x] != ' ')
                faces.push_back(Face(index++, x, y));

    for (auto &face : faces)
    {
        Position above, before;
        above.first = {face.left, face.top - 1};
        before.first = {face.left - 1, face.top};

        for (auto &connected : faces)
        {
            if (connected.has(above))
            {
                face.connected_top = connected.index;
                connected.connected_bottom = face.index;
            }
            if (connected.has(before))
            {
                face.connected_left = connected.index;
                connected.connected_right = face.index;
            }
        }
    }

    for (auto &face : faces)
    {
        std::optional<int> left_of_left;
        std::optional<int> right_of_right;
        std::optional<int> top_of_top;
        std::optional<int> bottom_of_bottom;

        int index[2] = {-1, -1};

        if (face.connected_left.has_value() &&
            face.connected_right.has_value())
        {
            const auto &left = face.connected_left.value();
            const auto &right = face.connected_right.value();
            index[0] = left;
            index[1] = right;
            left_of_left = faces[left].connected_left;
            right_of_right = faces[right].connected_right;
        }
        else if (face.connected_top.has_value() &&
                 face.connected_bottom.has_value())
        {
            const auto &top = face.connected_top.value();
            const auto &bottom = face.connected_bottom.value();
            index[0] = top;
            index[1] = bottom;
            top_of_top = faces[top].connected_top;
            bottom_of_bottom = faces[bottom].connected_bottom;
        }
        else
        {
            continue;
        }

        int corner_id = 0;
        for (int i = 0; i < 2; i++)
            for (int c = 0; c < 4; c++)
                faces[index[i]].corner_id[c] = corner_id++;

        if (face.connected_left.has_value() &&
            face.connected_right.has_value())
        {
            auto &left = faces[face.connected_left.value()];
            auto &right = faces[face.connected_right.value()];
            if (left.connected_top.has_value())
                faces[left.connected_top.value()].corner_id[0] =
                    right.corner_id[1];
            if (left.connected_bottom.has_value())
                faces[left.connected_bottom.value()].corner_id[2] =
                    right.corner_id[3];
            if (right.connected_top.has_value())
                faces[right.connected_top.value()].corner_id[1] =
                    left.corner_id[0];
            if (right.connected_bottom.has_value())
                faces[right.connected_bottom.value()].corner_id[3] =
                    left.corner_id[2];
        }
        else if (face.connected_top.has_value() &&
                 face.connected_bottom.has_value())
        {
            auto &top = faces[face.connected_top.value()];
            auto &bottom = faces[face.connected_bottom.value()];
            if (top.connected_left.has_value())
                faces[top.connected_left.value()].corner_id[0] =
                    bottom.corner_id[2];
            if (top.connected_right.has_value())
                faces[top.connected_right.value()].corner_id[1] =
                    bottom.corner_id[3];
            if (bottom.connected_left.has_value())
                faces[bottom.connected_left.value()].corner_id[2] =
                    top.corner_id[0];
            if (bottom.connected_right.has_value())
                faces[bottom.connected_right.value()].corner_id[3] =
                    top.corner_id[1];
        }

        if (left_of_left.has_value())
        {
            faces[left_of_left.value()].corner_id[0] =
                faces[index[1]].corner_id[1];
            faces[left_of_left.value()].corner_id[2] =
                faces[index[1]].corner_id[3];
        }
        if (right_of_right.has_value())
        {
            faces[right_of_right.value()].corner_id[1] =
                faces[index[0]].corner_id[0];
            faces[right_of_right.value()].corner_id[3] =
                faces[index[0]].corner_id[2];
        }
        if (top_of_top.has_value())
        {
            faces[top_of_top.value()].corner_id[0] =
                faces[index[1]].corner_id[2];
            faces[top_of_top.value()].corner_id[1] =
                faces[index[1]].corner_id[3];
        }
        if (bottom_of_bottom.has_value())
        {
            faces[bottom_of_bottom.value()].corner_id[2] =
                faces[index[0]].corner_id[0];
            faces[bottom_of_bottom.value()].corner_id[3] =
                faces[index[0]].corner_id[1];
        }

        if (!(left_of_left.has_value() || right_of_right.has_value()))
        {
            for (auto &face : faces)
            {
                if (face.block_x() - faces[index[0]].block_x() == 3)
                {
                    int by = face.block_y() - faces[index[0]].block_y();
                    if (by == 1)
                        face.corner_id[3] = faces[index[0]].corner_id[0];
                    else if (by == -1)
                        face.corner_id[1] = faces[index[0]].corner_id[2];
                }
                if (face.block_x() - faces[index[1]].block_x() == -3)
                {
                    int by = face.block_y() - faces[index[1]].block_y();
                    if (by == 1)
                        face.corner_id[2] = faces[index[1]].corner_id[1];
                    else if (by == -1)
                        face.corner_id[0] = faces[index[1]].corner_id[3];
                }
            }
        }

        if (!(top_of_top.has_value() || bottom_of_bottom.has_value()))
        {
            for (auto &face : faces)
            {
                if (face.block_y() - faces[index[0]].block_y() == 3)
                {
                    int bx = face.block_x() - faces[index[0]].block_x();
                    if (bx == 1)
                        face.corner_id[3] = faces[index[0]].corner_id[0];
                    else if (bx == -1)
                        face.corner_id[2] = faces[index[0]].corner_id[1];
                }
                if (face.block_y() - faces[index[1]].block_y() == -3)
                {
                    int bx = face.block_x() - faces[index[1]].block_x();
                    if (bx == 1)
                        face.corner_id[1] = faces[index[1]].corner_id[2];
                    else if (bx == -1)
                        face.corner_id[0] = faces[index[1]].corner_id[3];
                }
            }
        }
        break;
    }

    auto set_corner_id = [](std::optional<int> &id,
                            std::optional<int> &connected,
                            int id_connected) -> void
    {
        if (!id.has_value())
        {
            if (connected.has_value() &&
                faces[connected.value()].corner_id[id_connected].has_value())
            {
                id = faces[connected.value()].corner_id[id_connected].value();
            }
        }
    };

    int corner_ids = 0;
    while (true)
    {
        for (auto &face : faces)
        {
            set_corner_id(face.corner_id[0], face.connected_top, 2);
            set_corner_id(face.corner_id[0], face.connected_left, 1);
            set_corner_id(face.corner_id[1], face.connected_top, 3);
            set_corner_id(face.corner_id[1], face.connected_right, 0);
            set_corner_id(face.corner_id[2], face.connected_bottom, 0);
            set_corner_id(face.corner_id[2], face.connected_left, 3);
            set_corner_id(face.corner_id[3], face.connected_bottom, 1);
            set_corner_id(face.corner_id[3], face.connected_right, 2);
        }

        for (auto &face : faces)
        {
            std::optional<int> &left = face.connected_left;
            std::optional<int> &top = face.connected_top;
            std::optional<int> &right = face.connected_right;
            std::optional<int> &bottom = face.connected_bottom;

            if (right.has_value())
            {
                set_corner_id(face.corner_id[0],
                              faces[right.value()].connected_top, 0);
                set_corner_id(face.corner_id[2],
                              faces[right.value()].connected_bottom, 2);
            }
            if (left.has_value())
            {
                set_corner_id(face.corner_id[1],
                              faces[left.value()].connected_top, 1);
                set_corner_id(face.corner_id[3],
                              faces[left.value()].connected_bottom, 3);
            }
            if (top.has_value())
            {
                set_corner_id(face.corner_id[2],
                              faces[top.value()].connected_left, 2);
                set_corner_id(face.corner_id[3],
                              faces[top.value()].connected_right, 3);
            }
            if (bottom.has_value())
            {
                set_corner_id(face.corner_id[0],
                              faces[bottom.value()].connected_left, 0);
                set_corner_id(face.corner_id[1],
                              faces[bottom.value()].connected_right, 1);
            }
        }

        int c = 0;
        for (const auto &face : faces)
            c += face.number_of_corner_id();
        if (c == corner_ids)
            break;
        corner_ids = c;
    }

    if (corner_ids == 24)
    {
        for (auto &face : faces)
        {
            face.edges[Facing::Right] = {face.corner_id[1].value(),
                                         face.corner_id[3].value()};
            face.edges[Facing::Down] = {face.corner_id[2].value(),
                                        face.corner_id[3].value()};
            face.edges[Facing::Left] = {face.corner_id[0].value(),
                                        face.corner_id[2].value()};
            face.edges[Facing::Up] = {face.corner_id[0].value(),
                                      face.corner_id[1].value()};
        }
    }

#ifdef DPRINT_ON
    std::cout << "face size " << face_size << std::endl;
    for (int i = 0; i < faces.size(); i++)
    {
        auto &face = faces[i];
        std::cout << "face " << i << ": ";
        DPRINT2(face.left, face.top);
        std::cout << "-";
        DPRINT2(face.right, face.bottom);
        std::cout << " corners: ";
        for (const auto id : face.corner_id)
            if (id.has_value())
                std::cout << id.value() << " ";
            else
                std::cout << "  ";
        std::cout << "connected: ";
        if (face.connected_left.has_value())
            std::cout << face.connected_left.value() << " ";
        if (face.connected_top.has_value())
            std::cout << face.connected_top.value() << " ";
        if (face.connected_right.has_value())
            std::cout << face.connected_right.value() << " ";
        if (face.connected_bottom.has_value())
            std::cout << face.connected_bottom.value() << " ";
        if (corner_ids == 24)
        {
            std::cout << "edges: right";
            DPRINT2(face.edges[facing(0)].first,
                    face.edges[facing(0)].second);
            std::cout << " down";
            DPRINT2(face.edges[facing(1)].first,
                    face.edges[facing(1)].second);
            std::cout << " left";
            DPRINT2(face.edges[facing(2)].first,
                    face.edges[facing(2)].second);
            std::cout << " up";
            DPRINT2(face.edges[facing(3)].first,
                    face.edges[facing(3)].second);
        }
        DPRINT_ENDL();
    }
#endif
    return corner_ids == 24;
}

bool apply_wrapping_rules(Position &current, Position &moved)
{
    const auto current_face_index = index_face(current).value();
    auto &current_face = faces[current_face_index];
    const auto &current_edge = current_face.edges[position_facing(current)];
    auto edge_reversed = current_edge;
    std::swap(edge_reversed.first, edge_reversed.second);
    std::optional<int> moved_face_index;
    std::pair<int, int> moved_edge;
    Facing new_facing;

    for (const auto &face : faces)
    {
        if (face.index == current_face_index)
            continue;

        for (const auto &edge : face.edges)
        {
            if (edge.second == current_edge || edge.second == edge_reversed)
            {
                moved_face_index = face.index;
                moved_edge = edge.second;
                new_facing = facing_in(edge.first);
                break;
            }
        }

        if (moved_face_index.has_value())
            break;
    }

    if (!moved_face_index.has_value())
        return false;

    bool reversed = moved_edge == edge_reversed;
    int edge_offset = current_face.to_edge_offset(current);
    auto &moved_face = faces[moved_face_index.value()];
    moved_face.set_position(moved, edge_offset, reversed, new_facing);

#ifdef DPRINT_ON
    std::cout << "facing "
              << arrow[index(position_facing(current))]
              << " from ";
    DPRINT2(position_x(current), position_y(current));
    std::cout << "[face " << current_face_index
              << "] offset " << edge_offset;
    if (reversed)
        std::cout << " reversed";
    std::cout << " to ";
    DPRINT2(position_x(moved), position_y(moved));
    std::cout << "[face " << moved_face_index.value()
              << "] facing "
              << arrow[index(position_facing(moved))];
    DPRINT_ENDL();
#endif
    return true;
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
