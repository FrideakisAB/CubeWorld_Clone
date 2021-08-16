#ifndef MATH_H
#define MATH_H

#include <random>
#include "Engine.h"
#include "Platform.h"

namespace Math {
    template<typename T>
    T RangeRandom(T a, T b)
    {
        std::uniform_int_distribution<T> distrib(a, b);
        return distrib(GameEngine->GetRandomEngine());
    }
}

#endif
