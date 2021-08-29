#include "ECS/util/Timer.h"

namespace ECS::util {
    Timer::Timer() noexcept:
            elapsed(0), timeScale(1.0f), dt(1.0f / 60.0f), nonScaleDT(1.0f / 60.0f), last(std::chrono::high_resolution_clock::now()) {}

    void Timer::Tick() noexcept
    {
        auto ts = std::chrono::high_resolution_clock::now();

        nonScaleDT = std::chrono::duration_cast<std::chrono::nanoseconds>(ts - last).count() / 1000000000.0f;
        dt = nonScaleDT * timeScale;
        elapsed += std::chrono::duration<f32, std::milli>(dt);

        last = ts;
    }

    void Timer::Reset() noexcept
    {
        elapsed = Elapsed::zero();
    }

    void Timer::SetTimeScale(f32 ts) noexcept
    {
        timeScale = ts;
    }
}