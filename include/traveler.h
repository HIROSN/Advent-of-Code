#pragma once

/*
#include <debug_print.h>
#include <main.h>
#include <traveler.h>

//
// ForwardFour (default)
//
{
    aoc::Traveler traveler({0, 0}, {99, 99}, 100, 100);
    DPRINTX_ENDL(traveler);
    for (const auto &offset : traveler.direction.offsets())
    {
        traveler.visit(offset);
        DPRINTX_ENDL(traveler);
    }
}
// {0,0}
// {0,0}>{1,0}
// {0,0}>{1,0}v{1,1}
// {0,0}>{1,0}v{1,1}^{1,0}
// {0,0}>{1,0}v{1,1}^{1,0}<{0,0}

//
// StraightFirst (default)
//
{
    aoc::Traveler traveler({0, 0}, {99, 99}, 100, 100);
    DPRINTX_ENDL(traveler);
    traveler.visit(aoc::Offset::Down);
    DPRINTX_ENDL(traveler);
    for (int i = 0; i < 3; i++)
    {
        traveler.visit(traveler.direction.offsets()[0]);
        DPRINTX_ENDL(traveler);
    }
}
// {0,0}
// {0,0}v{0,1}
// {0,0}v{0,1}v{0,2}
// {0,0}v{0,1}v{0,2}v{0,3}
// {0,0}v{0,1}v{0,2}v{0,3}v{0,4}

//
// BackwardFour
//
{
    aoc::Traveler traveler({0, 0}, {99, 99}, 100, 100);
    traveler.direction.neighbors = aoc::BackwardFour();
    DPRINTX_ENDL(traveler);
    for (const auto &offset : traveler.direction.offsets())
    {
        traveler.visit(offset);
        DPRINTX_ENDL(traveler);
    }
}
// {0,0}
// {0,0}<{-1,0}
// {0,0}<{-1,0}^{-1,-1}
// {0,0}<{-1,0}^{-1,-1}v{-1,0}
// {0,0}<{-1,0}^{-1,-1}v{-1,0}>{0,0}

//
// TurnFirst
//
{
    aoc::Traveler traveler({0, 0}, {99, 99}, 100, 100);
    traveler.direction.type = aoc::Direction::Type::TurnFirst;
    DPRINTX_ENDL(traveler);
    for (const auto &offset : traveler.direction.offsets())
    {
        traveler.visit(offset);
        DPRINTX_ENDL(traveler);
    }
}
// {0,0}
// {0,0}v{0,1}
// {0,0}v{0,1}^{0,0}
// {0,0}v{0,1}^{0,0}>{1,0}
// {0,0}v{0,1}^{0,0}>{1,0}<{0,0}

//
// Priority::Far (default)
//
{
    aoc::Traveler traveler({0, 0}, {size_x - 1, size_y - 1}, size_x, size_y);

    std::priority_queue<std::pair<int, aoc::Traveler>,
                        std::vector<std::pair<int, aoc::Traveler>>,
                        std::greater<std::pair<int, aoc::Traveler>>>;
}
// +----------+
// | .........|
// |v.........|
// |v>>>>>>...|
// |......v>..|
// |.......v..|
// |.......v>.|
// |........v.|
// |........v.|
// |........v>|
// |.........v|
// +----------+
// 40

//
// Priority::Near
//
{
    aoc::Point::priority = aoc::Point::Priority::Near;
    aoc::Traveler traveler({0, 0}, {size_x - 1, size_y - 1}, size_x, size_y);

    std::priority_queue<std::pair<int, aoc::Traveler>,
                        std::vector<std::pair<int, aoc::Traveler>>,
                        std::less<std::pair<int, aoc::Traveler>>>;
}
// +----------+
// | .^>>>.^>>|
// |v>>..v>>.v|
// |.........v|
// |......<<<v|
// |......v...|
// |...<<<v...|
// |<<<v...^>>|
// |v......^.v|
// |v.^>>.^>.v|
// |v>>.v>>..v|
// +----------+
// 215

//
// Priority::Quick
//
{
    aoc::Point::priority = aoc::Point::Priority::Quick;
    aoc::Traveler traveler({0, 0}, {size_x - 1, size_y - 1}, size_x, size_y);

    std::priority_queue<std::pair<int, aoc::Traveler>,
                        std::vector<std::pair<int, aoc::Traveler>>,
                        std::greater<std::pair<int, aoc::Traveler>>>;
}
// +----------+
// |.>>>>>....|
// |.....v....|
// |.....v>...|
// |......v>..|
// |.......v>.|
// |........v.|
// |........v.|
// |........v.|
// |........v.|
// |........v>|
// +----------+
// 48

//
// Priority::Long
//
{
    aoc::Point::priority = aoc::Point::Priority::Long;
    aoc::Traveler traveler({0, 0}, {size_x - 1, size_y - 1}, size_x, size_y);

    std::priority_queue<std::pair<int, aoc::Traveler>,
                        std::vector<std::pair<int, aoc::Traveler>>,
                        std::greater<std::pair<int, aoc::Traveler>>>;
}
// +----------+
// |.>>>>>>>>>|
// |.........v|
// |.........v|
// |.........v|
// |.........v|
// |.........v|
// |.........v|
// |.........v|
// |.........v|
// |.........v|
// +----------+
// 75
*/

#include <map>
#include <utility>
#include <vector>

#define Number int

namespace aoc
{
    struct Point
    {
        Number x = 0;
        Number y = 0;

        Point() = default;
        Point(Number px, Number py) : x(px), y(py) {}

        enum Priority
        {
            Far,
            Near,
            Quick,
            Long,
        };

        virtual bool operator<(const Point &right) const
        {
            switch (priority)
            {
            case Priority::Far:
                return x < right.x || (!(right.x < x) && y < right.y);
            case Priority::Near:
                return x > right.x || (!(right.x > x) && y > right.y);
            case Priority::Quick:
                return x * y < right.x * right.y;
            default:
                break;
            }
            return false;
        }

        virtual bool operator==(const Point &right) const
        {
            return x == right.x && y == right.y;
        }

        virtual bool operator>(const Point &right) const
        {
            return !(*this < right || *this == right);
        }

        virtual bool operator!=(const Point &right) const
        {
            return !(*this == right);
        }

        static Priority priority;

        friend std::ostream &operator<<(
            std::ostream &out, const Point &point);
    };

    Point::Priority Point::priority = Point::Priority::Far;

    struct Offset : public Point
    {
        char ch = 0;

        Offset() = default;
        Offset(Number ox, Number oy, char oc) : Point(ox, oy), ch(oc) {}

        virtual bool operator==(const Offset &right) const
        {
            return ch == right.ch;
        }

        virtual bool operator==(char right) const
        {
            return ch == right;
        }

        static Offset Right;
        static Offset Down;
        static Offset Left;
        static Offset Up;
        static Offset Stay;

        friend std::ostream &operator<<(
            std::ostream &out, const Offset &offset);
    };

    Offset Offset::Right = {1, 0, '>'};
    Offset Offset::Down = {0, 1, 'v'};
    Offset Offset::Left = {-1, 0, '<'};
    Offset Offset::Up = {0, -1, '^'};
    Offset Offset::Stay = {0, 0, 0};

    struct Neighbors
    {
        static int number_of_offsets;
        static std::vector<Offset> offsets;

        friend std::ostream &operator<<(
            std::ostream &out, const Neighbors &neighbors);
    };

    int Neighbors::number_of_offsets = 0;
    std::vector<Offset> Neighbors::offsets;

    struct FourNeighbors : public Neighbors
    {
        FourNeighbors()
        {
            number_of_offsets = 4;
        }
    };

    struct ForwardFour : public FourNeighbors
    {
        ForwardFour() : FourNeighbors()
        {
            offsets = {Offset::Right, Offset::Down, Offset::Left, Offset::Up};
        }
    };

    struct BackwardFour : public FourNeighbors
    {
        BackwardFour() : FourNeighbors()
        {
            offsets = {Offset::Left, Offset::Up, Offset::Right, Offset::Down};
        }
    };

    struct Direction
    {
        Offset heading = Offset::Stay;

        enum Type
        {
            StraightFirst,
            TurnFirst,
        };

        Direction()
        {
            type = Type::StraightFirst;
            neighbors = ForwardFour();
        }

        virtual int neighbor_index() const
        {
            for (int i = 0; i < neighbors.number_of_offsets; i++)
                if (heading == neighbors.offsets[i])
                    return i;
            return 0;
        }

        virtual std::vector<Offset> offsets(
            int max_size = neighbors.number_of_offsets)
        {
            std::vector<Offset> offsets;
            const int index_straight = neighbor_index();

            if (index_straight < neighbors.number_of_offsets)
            {
                auto fix = [&](int index) -> int
                {
                    int size = neighbors.number_of_offsets;
                    return (index + size) % size;
                };

                const int index_turn_right = fix(index_straight + 1);
                const int index_turn_left = fix(index_straight - 1);

                switch (type)
                {
                case Type::StraightFirst:
                    offsets.push_back(neighbors.offsets[index_straight]);
                    offsets.push_back(neighbors.offsets[index_turn_right]);
                    offsets.push_back(neighbors.offsets[index_turn_left]);
                    break;
                case Type::TurnFirst:
                    offsets.push_back(neighbors.offsets[index_turn_right]);
                    offsets.push_back(neighbors.offsets[index_turn_left]);
                    offsets.push_back(neighbors.offsets[index_straight]);
                    break;
                }

                for (int i = 0; i < max_size; i++)
                {
                    if (i != index_straight &&
                        i != index_turn_right &&
                        i != index_turn_left)
                    {
                        offsets.push_back(neighbors.offsets[i]);
                    }
                }
            }
            return offsets;
        }

        static Type type;
        static Neighbors neighbors;
    };

    Direction::Type Direction::type = Type::StraightFirst;
    Neighbors Direction::neighbors = ForwardFour();

    struct Traveler
    {
        Number x = 0;
        Number y = 0;
        Direction direction;
        std::vector<std::pair<Offset, Point>> visited;

        Traveler() = default;

        Traveler(const Point &ps, const Point &pe, Number sx, Number sy)
        {
            x = ps.x;
            y = ps.y;
            start = ps;
            end = pe;
            size_x = sx;
            size_y = sy;
            visited.push_back({direction.heading, start});
        }

        virtual bool valid(const Offset &offset) const
        {
            const auto ax = x + offset.x;
            const auto ay = y + offset.y;
            return ax >= 0 && ax < size_x &&
                   ay >= 0 && ay < size_y;
        }

        virtual bool is_visited(const Offset &offset) const
        {
            const Point point(x + offset.x, y + offset.y);
            for (auto it : visited)
                if (it.second == point)
                    return true;
            return false;
        }

        virtual void visit(const Offset &offset)
        {
            x += offset.x;
            y += offset.y;
            direction.heading = offset;
            visited.push_back({offset, {x, y}});
        }

        virtual void visit(Number ax, Number ay)
        {
            x = ax;
            y = ay;
        }

        virtual Point is_at() const
        {
            return Point(x, y);
        }

        virtual bool is_at_end() const
        {
            return is_at() == end;
        }

        virtual std::map<std::pair<int, int>, char> get_path(
            char ch_space, char ch_path = 0) const
        {
            std::map<std::pair<int, int>, char> path{
                {{0, 0}, ch_space}, {{size_x - 1, size_y - 1}, ch_space}};
            for (auto it = visited.rbegin(); it != visited.rend(); it++)
                path[{it->second.x, it->second.y}] =
                    ch_path ? ch_path : it->first.ch ? it->first.ch
                                                     : ch_space;
            return path;
        }

        virtual bool operator<(const Traveler &right) const
        {
            switch (Point::priority)
            {
            case Point::Priority::Long:
                return visited.size() < right.visited.size();
            default:
                return Point(x, y) < Point(right.x, right.y);
            }
            return false;
        }

        virtual bool operator==(const Traveler &right) const
        {
            return Point(x, y) == Point(right.x, right.y);
        }

        virtual bool operator>(const Traveler &right) const
        {
            return !(*this < right || *this == right);
        }

        virtual bool operator!=(const Traveler &right) const
        {
            return !(*this == right);
        }

        static Point start;
        static Point end;
        static Number size_x;
        static Number size_y;

        friend std::ostream &operator<<(
            std::ostream &out, const aoc::Traveler &traveler);
    };

    Point Traveler::start = {};
    Point Traveler::end = {};
    Number Traveler::size_x = 0;
    Number Traveler::size_y = 0;

    std::ostream &operator<<(std::ostream &out, const Point &point)
    {
#ifdef DPRINT_ON
        out << "{" << point.x << "," << point.y << "}";
#endif
        return out;
    }

    std::ostream &operator<<(std::ostream &out, const Offset &offset)
    {
#ifdef DPRINT_ON
        out << "{" << offset.ch << ":" << offset.x << "," << offset.y << "}";
#endif
        return out;
    }

    std::ostream &operator<<(std::ostream &out, const Neighbors &neighbors)
    {
#ifdef DPRINT_ON
        for (const auto &offset : neighbors.offsets)
            out << offset;
#endif
        return out;
    }

    std::ostream &operator<<(std::ostream &out, const Traveler &traveler)
    {
#ifdef DPRINT_ON
        for (const auto &visit : traveler.visited)
            out << visit.first.ch << visit.second;
#endif
        return out;
    }

} // namespace aoc
