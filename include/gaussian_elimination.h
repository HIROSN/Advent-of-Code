#pragma once

#include <stdint.h>
#include <vector>

#ifndef Number
#define Number int64_t
#endif

using Matrix = std::vector<std::vector<double>>;

bool gaussian_elimination(Matrix &matrix)
{
#ifdef DPRINT_ON
    for (const auto &r : matrix)
    {
        std::cout << "[";
        Number c = 0;
        for (const auto &v : r)
            std::cout << (c++ ? " " : "") << v;
        std::cout << "]" << std::endl;
    }
#endif
    auto &a = matrix;
    const Number N = matrix.size();
    /*
     * Copyright 2021 Hiroyuki Chishiro
     * License: 2-Clause BSD
     * https://hiroyukichishiro.com/simultaneous-linear-equations-in-c-language/
     *
     * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
     * CONTRIBUTORS “AS IS” AND ANY EXPRESS OR IMPLIED WARRANTIES,
     * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
     * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
     * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
     * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
     * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
     * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
     * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
     * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
     * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
     * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
     * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
     */
    double p, d;
    Number i, j, k;

    for (i = 0; i < N; i++)
    {
        p = a[i][i];

        if (p == 0.0)
            return false;

        for (j = 0; j < (N + 1); j++)
        {
            a[i][j] = a[i][j] / p;
        }

        for (j = 0; j < N; j++)
        {
            if (i != j)
            {
                d = a[j][i];

                for (k = i; k < (N + 1); k++)
                {
                    a[j][k] = a[j][k] - d * a[i][k];
                }
            }
        }
    }

#ifdef DPRINT_ON
    Number c = 0;
    std::cout << "{";
    for (const auto &r : matrix)
        if (r.size())
            std::cout << (c++ ? "," : "") << r[r.size() - 1];
    std::cout << "}" << std::endl;
#endif
    return true;
}
