#ifndef ECS_TIMER_H
#define ECS_TIMER_H

#include <chrono>
#include "ECS/API.h"

namespace ECS::util {
    class Timer {
        using Elapsed = std::chrono::duration<f32, std::milli>;
        using TimePoint = std::chrono::high_resolution_clock::time_point;

    private:
        Elapsed elapsed;
        f32 timeScale;
        f32 dt, nonScaleDT;
        TimePoint last;

    public:
        Timer() noexcept;

        void Tick() noexcept;
        void Reset() noexcept;

        [[nodiscard]] inline TimeStamp GetTimeStamp() const noexcept { return TimeStamp(elapsed.count()); }
        [[nodiscard]] inline f32 GetWorkTime() const noexcept { return elapsed.count(); }

        void SetTimeScale(f32 ts) noexcept;

        [[nodiscard]] inline f32 GetTimeScale() const noexcept { return timeScale; }

        [[nodiscard]] inline f32 GetDeltaTime() const noexcept { return dt; }
        [[nodiscard]] inline f32 GetNonScaleDeltaTime() const noexcept { return nonScaleDT; }
    };
}

#endif

