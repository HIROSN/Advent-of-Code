// #include <debug_print.h>
#include <main.h>

#include <utility>
#include <vector>

using Point = std::pair<int, int>;

static const std::vector<std::vector<Point>> shapes = {
    {{0, 0}, {1, 0}, {2, 0}, {3, 0}},
    {{1, 0}, {0, 1}, {1, 1}, {2, 1}, {1, 2}},
    {{2, 0}, {2, 1}, {0, 2}, {1, 2}, {2, 2}},
    {{0, 0}, {0, 1}, {0, 2}, {0, 3}},
    {{0, 0}, {1, 0}, {0, 1}, {1, 1}},
};

static const std::string space(7, '.');
std::vector<std::string> chamber;

struct Shape
{
    Point top_left = {2, 0};
    std::vector<Point> rocks;
    bool rest = false;

    Shape(int index)
    {
        rocks = shapes[index];
    }

    int height() const
    {
        return rocks.rbegin()->second - rocks.begin()->second + 1;
    }

    void draw(char as) const
    {
        for (const auto &rock : rocks)
        {
            int x = top_left.first + rock.first;
            int y = top_left.second + rock.second;
            chamber[y][x] = as;
        }
    }

    void move(char jet)
    {
        int left = top_left.first;
        int top = top_left.second;
        bool can_move = true;

        switch (jet)
        {
        case '<':
            left--;
            break;
        case '>':
            left++;
            break;
        case 'v':
            top++;
            break;
        }

        for (const auto &rock : rocks)
        {
            int x = left + rock.first;
            int y = top + rock.second;
            if (y >= 0 && y < chamber.size() &&
                x >= 0 && x < chamber[0].size())
            {
                if (chamber[y][x] == '#')
                {
                    can_move = false;
                    break;
                }
            }
            else
            {
                can_move = false;
                break;
            }
        }

        if (can_move)
        {
            draw('.');
            top_left = {left, top};
            draw('@');

            auto it_top = chamber.begin();
            if (*it_top == space)
            {
                chamber.erase(it_top);
                top_left.second--;
            }
        }
        else if (jet == 'v')
        {
            draw('#');
            rest = true;
        }
    }
};

std::optional<uint64_t> Answer(std::ifstream &file)
{
    std::vector<char> moves;
    char ch;
    while (file >> ch)
        moves.push_back(ch);

    int index_move = 0;
    int index_shape = 0;

    for (int rock = 1; rock <= 2022; rock++)
    {
        Shape shape(index_shape);
        chamber.insert(chamber.begin(), shape.height() + 3, space);
        shape.draw('@');
        // DPRINT(chamber);

        while (!shape.rest)
        {
            shape.move(moves[index_move]);
            shape.move('v');
            // DPRINT(chamber);
            index_move = ++index_move % moves.size();
        }
        index_shape = ++index_shape % shapes.size();
    }
    DPRINT(chamber);

    return chamber.size();
}
