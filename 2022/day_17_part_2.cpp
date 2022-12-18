#include <debug_print.h>
#undef DPRINT_ON
#include <main.h>

#include <cmath>
#include <list>
#include <map>
#include <utility>
#include <vector>

struct Shape;
using Point = std::pair<int64_t, int64_t>;

static const std::vector<std::pair<std::vector<Point>,
                                   std::pair<int, int>>>
    shapes = {
        {{{0, 0}, {1, 0}, {2, 0}, {3, 0}}, {4, 1}},
        {{{1, 0}, {0, -1}, {1, -1}, {2, -1}, {1, -2}}, {3, 3}},
        {{{2, 0}, {2, -1}, {0, -2}, {1, -2}, {2, -2}}, {3, 3}},
        {{{0, 0}, {0, -1}, {0, -2}, {0, -3}}, {1, 4}},
        {{{0, 0}, {1, 0}, {0, -1}, {1, -1}}, {2, 2}},
};

std::list<Shape> shapes_at_rest;
std::vector<char> moves;
int64_t highest_top = -1;

struct Shape
{
    Point top_left = {};
    int index_shape = 0;
    int id = 0;
    int64_t tower_height = 0;

    Shape() = default;

    Shape(int index)
    {
        index_shape = index;
    }

    const std::vector<Point> &rocks() const
    {
        return shapes[index_shape].first;
    }

    int width() const
    {
        return shapes[index_shape].second.first;
    }

    int height() const
    {
        return shapes[index_shape].second.second;
    }

    bool good() const
    {
        if (top_left.first < 0 ||
            top_left.first + width() - 1 >= 7 ||
            top_left.second - height() + 1 < 0)
        {
            return false;
        }
        return true;
    }

    bool move(char jet)
    {
        auto copy = *this;
        switch (jet)
        {
        case '<':
            copy.top_left.first--;
            break;
        case '>':
            copy.top_left.first++;
            break;
        case 'v':
            copy.top_left.second--;
            break;
        }

        bool can_move = copy.good();
        if (can_move)
        {
            for (const auto &rest : shapes_at_rest)
            {
                if (copy && rest)
                {
                    can_move = false;
                    break;
                }
            }
        }

        if (can_move)
        {
            top_left = copy.top_left;
        }
        else if (jet == 'v')
        {
            tower_height = highest_top + 1;
            shapes_at_rest.push_front(*this);
            return false;
        }
        return true;
    }

    bool operator&&(const Shape &r) const
    {
        if (std::abs(top_left.first - r.top_left.first) >= 4 ||
            std::abs(top_left.second - r.top_left.second) >= 4)
        {
            return false;
        }
        for (const auto &lrock : rocks())
        {
            for (const auto &rrock : r.rocks())
            {
                if (top_left.first + lrock.first ==
                        r.top_left.first + rrock.first &&
                    top_left.second + lrock.second ==
                        r.top_left.second + rrock.second)
                {
                    return true;
                }
            }
        }
        return false;
    }
};

void let_it_fall(int64_t rock, int &index_move, int &index_shape);

std::vector<std::string> draw_chamber(
    std::vector<std::vector<int>> &rock_id,
    std::vector<std::vector<int>> &tower_height);

std::optional<uint64_t> Answer(std::ifstream &file)
{
    char ch;
    while (file >> ch)
        moves.push_back(ch);

    int index_move = 0;
    int index_shape = 0;
    int64_t rock = 0;

    while (++rock)
    {
        let_it_fall(rock, index_move, index_shape);
        if (rock == 5000)
            break;
    }

    std::vector<std::vector<int>> rock_id;
    std::vector<std::vector<int>> tower_height;
    std::vector<std::string> chamber = draw_chamber(rock_id, tower_height);

    std::vector<std::string> pattern;
    const int pattern_height = 10;
    for (int i = chamber.size() / 2; pattern.size() < pattern_height; i++)
        pattern.push_back(chamber[i]);
    DPRINT(pattern);

    int rock_id_last, tower_height_last;
    int rock_id_cycle, tower_height_cycle;

    int match = 0, p = 0, x = 0, prev_id, prev_height;
    for (int i = 0; i < chamber.size() - pattern.size(); i++)
    {
        if (chamber[i] == pattern[p])
            p++;
        if (p == pattern_height)
        {
            if (++match > 3)
                break;
            std::vector<std::vector<int>> pattern_id;
            std::vector<std::vector<int>> pattern_tower_height;
            for (int m = i - pattern_height; m < i; m++)
            {
                pattern_id.push_back(rock_id[m]);
                pattern_tower_height.push_back(tower_height[m]);
            }
            DPRINT(pattern_id);
            DPRINT(pattern_tower_height);

            switch (match)
            {
            case 1:
                while (!pattern_id[0][x] && x < pattern_id[0].size())
                    x++;
                rock_id_last = pattern_id[0][x];
                tower_height_last = pattern_tower_height[0][x];
                DPRINT2(rock_id_last, tower_height_last);
                DPRINT_ENDL();
                break;
            case 2:
                prev_id = pattern_id[0][x];
                prev_height = pattern_tower_height[0][x];
                DPRINT2(prev_id, prev_height);
                rock_id_cycle = rock_id_last - prev_id;
                tower_height_cycle = tower_height_last - prev_height;
                DPRINT2(rock_id_cycle, tower_height_cycle);
                DPRINT_ENDL();
                break;
            case 3:
                DPRINT2(prev_id - pattern_id[0][x],
                        prev_height - pattern_tower_height[0][x]);
                DPRINT_ENDL();
                if (rock_id_cycle == prev_id - pattern_id[0][x] &&
                    tower_height_cycle == prev_height -
                                              pattern_tower_height[0][x])
                {
                    break;
                }
                return {};
            }
            p = 0;
        }
    }
    DPRINT2(rock_id_last, tower_height_last);
    DPRINT2(rock_id_cycle, tower_height_cycle);
    DPRINT_ENDL();

    int64_t rocks_to_next_match = rock_id_last + rock_id_cycle - rock;
    int64_t remaining = 1000000000000 - (rock_id_last + rock_id_cycle);
    int64_t rocks_to_end = remaining % rock_id_cycle;
    int64_t cycles_skipping = remaining / rock_id_cycle;
    DPRINT2(rocks_to_next_match, rocks_to_end);
    DPRINT_ENDL();

    int64_t stop_at = rock + rocks_to_next_match + rocks_to_end;
    while (++rock)
    {
        let_it_fall(rock, index_move, index_shape);
        if (rock == stop_at)
            break;
    }

    return cycles_skipping * tower_height_cycle + highest_top + 1;
}

void let_it_fall(int64_t rock, int &index_move, int &index_shape)
{
    Shape shape(index_shape);
    shape.id = rock;
    shape.top_left = {2, highest_top + shape.height() + 3};

    while (true)
    {
        shape.move(moves[index_move]);
        index_move = ++index_move % moves.size();
        if (!shape.move('v'))
        {
            if (shape.top_left.second > highest_top)
                highest_top = shape.top_left.second;
            break;
        }
    }
    index_shape = ++index_shape % shapes.size();
}

std::vector<std::string> draw_chamber(
    std::vector<std::vector<int>> &rock_id,
    std::vector<std::vector<int>> &tower_height)
{
    std::map<std::pair<int, int>, char> mpc = {
        {{0, 0}, '.'},
        {{6, 0}, '.'},
    };

    std::map<std::pair<int, int>, int> mpi_id = {
        {{0, 0}, 0},
        {{6, 0}, 0},
    };

    std::map<std::pair<int, int>, int> mpi_th = mpi_id;

    for (const auto &shape : shapes_at_rest)
    {
        for (const auto &rock : shape.rocks())
        {
            int x = static_cast<int>(shape.top_left.first + rock.first);
            int y = static_cast<int>(shape.top_left.second + rock.second);
            mpc[{x, y}] = '#';
            mpi_id[{x, y}] = shape.id;
            mpi_th[{x, y}] = static_cast<int>(shape.tower_height);
        }
    }

    rock_id = mpi_to_vi(mpi_id, 0, -1);
    tower_height = mpi_to_vi(mpi_th, 0, -1);
    return mpc_to_vs(mpc, '.', -1);
}
