#include <main.h>

#include <map>
#include <utility>
#include <vector>

#define X 0
#define Y 1

std::optional<uint64_t> Answer(std::ifstream &file)
{
    using Point = std::pair<int, int>;

    struct Trail
    {
        Point head;
        Point tail;

        Trail(const Point &start) : head(start), tail(start) {}
    };

    struct HikingGuide
    {
        std::vector<std::string> topographic_map;
        std::vector<Point> highest_positions;
        int size_x;
        int size_y;
        const std::vector<Point> offsets{{0, -1}, {1, 0}, {0, 1}, {-1, 0}};

        void walk_back(const Trail &trail, std::vector<Trail> &trail_list)
        {
            int h = height(trail.head);
            if (h == 0)
            {
                trail_list.push_back(trail);
                return;
            }
            for (const auto &neighbor : neighbors(trail.head))
            {
                if (height(neighbor) == h - 1)
                {
                    auto trail_copy = trail;
                    trail_copy.head = neighbor;
                    walk_back(trail_copy, trail_list);
                }
            }
        }

        int height(const Point &p)
        {
            return topographic_map[std::get<Y>(p)][std::get<X>(p)] - '0';
        }

        bool is_valid(const Point &p)
        {
            int x = std::get<X>(p);
            int y = std::get<Y>(p);
            return x >= 0 && x < size_x && y >= 0 && y < size_y;
        }

        std::vector<Point> neighbors(const Point &p)
        {
            std::vector<Point> points;
            for (const auto &o : offsets)
            {
                int x = std::get<X>(p) + std::get<X>(o);
                int y = std::get<Y>(p) + std::get<Y>(o);
                if (is_valid({x, y}))
                    points.push_back({x, y});
            }
            return points;
        }
    };

    HikingGuide guide;
    std::string line;

    while (std::getline(file, line))
    {
        const int y = guide.topographic_map.size();
        guide.topographic_map.push_back(line);
        for (int x = 0; x < line.size(); x++)
            if (guide.height({x, y}) == 9)
                guide.highest_positions.push_back({x, y});
    }

    guide.size_x = line.size();
    guide.size_y = guide.topographic_map.size();
    std::vector<Trail> trail_list;
    std::map<Point, std::map<Point, int>> scores;
    uint64_t sum_of_scores = 0;

    for (const auto &tail : guide.highest_positions)
        guide.walk_back(Trail(tail), trail_list);

    for (const auto &trail : trail_list)
        scores[trail.head][trail.tail]++;

    for (auto it : scores)
        sum_of_scores += it.second.size();

    return sum_of_scores;
}
