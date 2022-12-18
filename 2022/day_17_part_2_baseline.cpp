// #include <debug_print.h>
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

constexpr int64_t number_of_rocks = 2022;
constexpr bool large_input = number_of_rocks >= 100000;
std::list<Shape> shapes_at_rest;

struct Shape
{
    Point top_left = {};
    int index_shape = 0;

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

int64_t highest_top = -1;

void trim_shapes_at_rest(const Shape &rest);

#ifdef DPRINT_ON
void debug_print();
#endif

std::optional<uint64_t> Answer(std::ifstream &file)
{
    std::vector<char> moves;
    char ch;
    while (file >> ch)
        moves.push_back(ch);

    int index_move = 0;
    int index_shape = 0;

    for (int64_t rock = 1; rock <= number_of_rocks; rock++)
    {
        Shape shape(index_shape);
        shape.top_left = {2, highest_top + shape.height() + 3};

        while (true)
        {
            shape.move(moves[index_move]);
            index_move = ++index_move % moves.size();
            if (!shape.move('v'))
            {
                if (shape.top_left.second > highest_top)
                    highest_top = shape.top_left.second;
                if (large_input)
                    trim_shapes_at_rest(shape);
                break;
            }
        }
        index_shape = ++index_shape % shapes.size();
    }
#ifdef DPRINT_ON
    // debug_print();
#endif

    return highest_top + 1;
}

void trim_shapes_at_rest(const Shape &rest)
{
    int64_t lower = rest.top_left.second - rest.height();
    int64_t upper = lower + 1;

    while (lower >= 0 && upper <= rest.top_left.second + 1)
    {
        std::map<int, bool> filled;
        for (const auto &shape : shapes_at_rest)
        {
            if (std::abs(rest.top_left.first - shape.top_left.first) >= 4 ||
                std::abs(rest.top_left.second - shape.top_left.second) >= 4)
            {
                continue;
            }

            for (const auto &rock : shape.rocks())
            {
                if (shape.top_left.second + rock.second == lower ||
                    shape.top_left.second + rock.second == upper)
                {
                    filled[shape.top_left.first + rock.first] = true;
                }
            }

            if (filled.size() == 7)
                break;
        }
        if (filled.size() == 7)
        {
            for (auto it = shapes_at_rest.begin();
                 it != shapes_at_rest.end();)
            {
                if (it->top_left.second < lower)
                    it = shapes_at_rest.erase(it);
                else
                    it++;
            }
            return;
        }

        lower++;
        upper++;
    }
}

#ifdef DPRINT_ON
void debug_print()
{
    std::map<std::pair<int, int>, char> map = {
        {{0, 0}, '.'},
        {{6, 0}, '.'},
    };
    for (const auto &shape : shapes_at_rest)
    {
        for (const auto &rock : shape.rocks())
        {
            auto x = shape.top_left.first + rock.first;
            auto y = shape.top_left.second + rock.second;
            map[{static_cast<int>(x), static_cast<int>(y)}] = '#';
        }
    }
    DPRINT2(map, '.');
}
#endif
