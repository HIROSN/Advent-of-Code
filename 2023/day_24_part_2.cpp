// #include <debug_print.h>
#include <main.h>
#include <gaussian_elimination.h>

#include <vector>

struct Hailstone
{
    double px, py, pz;
    float vx, vy, vz;
};

std::optional<uint64_t> Answer(std::ifstream &file)
{
    std::vector<Hailstone> hailstones;
    Hailstone h;
    char ch;

    while (file >>
           h.px >> ch >> h.py >> ch >> h.pz >> ch >>
           h.vx >> ch >> h.vy >> ch >> h.vz)
    {
        hailstones.push_back(h);
    }

    auto add_row = [](Matrix &matrix,
                      double pix, double piy, double piz,
                      float vix, float viy, float viz,
                      double pjx, double pjy, double pjz,
                      float vjx, float vjy, float vjz) -> void
    {
        matrix.push_back(std::vector<double>{
            (viy - vjy) * (piz - pjz) - (viz - vjz) * (piy - pjy),
            (viz - vjz) * (pix - pjx) - (vix - vjx) * (piz - pjz),
            (vix - vjx) * (piy - pjy) - (viy - vjy) * (pix - pjx),
            (viz - vjz) * (pix * pjy - pjx * piy) +
                (viy - vjy) * (piz * pjx - pjz * pix) +
                (vix - vjx) * (piy * pjz - pjy * piz)});
    };

    for (auto h1 = hailstones.begin(); h1 != hailstones.end(); h1++)
    {
        for (auto h2 = h1 + 1; h2 != hailstones.end(); h2++)
        {
            for (auto h3 = h2 + 1; h3 != hailstones.end(); h3++)
            {
                Matrix matrix;
                add_row(matrix,
                        h1->px, h1->py, h1->pz, h1->vx, h1->vy, h1->vz,
                        h2->px, h2->py, h2->pz, h2->vx, h2->vy, h2->vz);
                add_row(matrix,
                        h2->px, h2->py, h2->pz, h2->vx, h2->vy, h2->vz,
                        h3->px, h3->py, h3->pz, h3->vx, h3->vy, h3->vz);
                add_row(matrix,
                        h3->px, h3->py, h3->pz, h3->vx, h3->vy, h3->vz,
                        h1->px, h1->py, h1->pz, h1->vx, h1->vy, h1->vz);

                if (gaussian_elimination(matrix))
                    return matrix[0][3] + matrix[1][3] + matrix[2][3];
            }
        }
    }

    return {};
}
