#pragma once
#include <random>

namespace pg {

pg::fVec2 getRandomVector()
{
    static std::random_device                    rd;
    static std::mt19937                          gen(rd());
    static std::uniform_real_distribution<float> dis(-1.0f, 1.0f);

    return makeNormal(pg::fVec2{dis(gen), dis(gen)});
}

template <typename T>
T randomBetween(T min, T max)
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

} // namespace pg