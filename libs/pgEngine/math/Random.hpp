#pragma once
#include <pgEngine/math/Vec.hpp>
#include <pgEngine/math/Random.hpp>
#include <numbers>
#include <random>

namespace pg {

static pg::fVec2 getRandomVector()
{
    static std::random_device                    rd;
    static std::mt19937                          gen(rd());
    static std::uniform_real_distribution<float> dis(-1.0f, 1.0f);

    return makeNormal(pg::fVec2{dis(gen), dis(gen)});
}

template <typename T>
static T randomBetween(T min, T max)
{
    if constexpr (std::is_floating_point<T>::value)
    {
        static std::random_device         rd;
        static std::mt19937               gen(rd());
        std::uniform_real_distribution<T> dis(min, max);
        return dis(gen);
    }
    if constexpr (std::is_integral<T>::value)
    {
        static std::random_device        rd;
        static std::mt19937              gen(rd());
        std::uniform_int_distribution<T> dis(min, max);
        return dis(gen);
    }
}

static pg::fVec2 getPointInCircle(double radius)
{
    auto a = randomBetween(0.0, 1.0);
    auto b = randomBetween(0.0, 1.0);
    if (b < a) { std::swap(a, b); }

    return {static_cast<float>(b * radius * std::cos(2.0f * std::numbers::pi * a / b)),
            static_cast<float>(b * radius * std::sin(2.0f * std::numbers::pi * a / b))};
}

} // namespace pg