// #include <debug_print.h>
#include <main.h>

#include <vector>

struct Hailstone
{
    double px, py, pz;
    float vx, vy, vz;

    friend std::ostream &operator<<(std::ostream &out, const Hailstone &h);
};

template <typename T>
int sgn(T val)
{
    return (T(0) < val) - (val < T(0));
}

std::optional<uint64_t> Answer(std::ifstream &file)
{
    std::vector<Hailstone> hailstones;
    int64_t at_least = 200000000000000, at_most = 400000000000000;
    {
        Hailstone h;
        char ch;

        while (file >>
               h.px >> ch >> h.py >> ch >> h.pz >> ch >>
               h.vx >> ch >> h.vy >> ch >> h.vz)
        {
            hailstones.push_back(h);
        }
    }

    auto within_area = [&](const double &c) -> bool
    {
        return c >= static_cast<double>(at_least) &&
               c <= static_cast<double>(at_most);
    };

    auto crossed_in_the_past = [](const Hailstone &h,
                                  double cx, double cy) -> bool
    {
        return sgn(cx - h.px) != sgn(h.vx) || sgn(cy - h.py) != sgn(h.vy);
    };

    int intersections_within_area = 0;

    for (auto h1 = hailstones.begin(); h1 != hailstones.end(); h1++)
    {
        for (auto h2 = h1 + 1; h2 != hailstones.end(); h2++)
        {
            DPRINTX_ENDL(*h1);
            DPRINTX_ENDL(*h2);

            auto a1 = h1->vy / h1->vx;
            auto b1 = h1->py - (a1 * h1->px);

            auto a2 = h2->vy / h2->vx;
            auto b2 = h2->py - (a2 * h2->px);

            if (a1 == a2)
                continue;

            auto cx = (b2 - b1) / (a1 - a2);
            auto cy = a1 * cx + b1;

            if (!within_area(cx) || !within_area(cy))
                continue;

            if (crossed_in_the_past(*h1, cx, cy) ||
                crossed_in_the_past(*h2, cx, cy))
            {
                continue;
            }

            intersections_within_area++;
            DPRINTX_ENDL("cross inside at {"
                         << static_cast<int64_t>(cx) << ","
                         << static_cast<int64_t>(cy) << "}");
        }
    }

    return intersections_within_area;
}

std::ostream &operator<<(std::ostream &out, const Hailstone &h)
{
    out << "{"
        << static_cast<int64_t>(h.px) << ","
        << static_cast<int64_t>(h.py) << ","
        << static_cast<int64_t>(h.pz)
        << "}";
    out << "["
        << static_cast<int>(h.vx) << ","
        << static_cast<int>(h.vy) << ","
        << static_cast<int>(h.vz)
        << "]";
    return out;
}
