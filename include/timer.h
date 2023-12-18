#pragma once

#include <chrono>

namespace aoc
{
    struct Timer
    {
        using Clock = std::chrono::high_resolution_clock;
        using Second = std::chrono::duration<double, std::ratio<1>>;

        std::chrono::time_point<Clock> beg = Clock::now();

        double elapsed() const
        {
            return std::chrono::duration_cast<Second>(
                       Clock::now() - beg)
                .count();
        }
    };

} // namespace aoc
