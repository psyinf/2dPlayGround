#pragma once
#include <pgEngine/math/Vec.hpp>
#include <pgEngine/math/VecOps.hpp>
#include <random>

namespace pg {

class SeedGenerator
{
public:
    SeedGenerator()
      : _gen(std::random_device{}())
    {
    }

    SeedGenerator(uint32_t fixed_seed)
      : _gen(fixed_seed)
    {
    }

    auto get() const -> std::mt19937& { return _gen; }

private:
    mutable std::mt19937 _gen;
};

static inline auto DefaultSeedGenerator = SeedGenerator();

inline pg::fVec2 getRandomVector(const SeedGenerator& gen = DefaultSeedGenerator)
{
    static std::uniform_real_distribution<float> dis(-1.0f, 1.0f);

    return makeNormal(pg::fVec2{dis(gen.get()), dis(gen.get())});
}

template <typename T>
inline T randomBetween(T min, T max, const SeedGenerator& gen = SeedGenerator())
{
    if constexpr (std::is_floating_point<T>::value)
    {
        std::uniform_real_distribution<T> dis(min, max);
        return dis(gen.get());
    }
    if constexpr (std::is_integral<T>::value)
    {
        std::uniform_int_distribution<T> dis(min, max);
        return dis(gen.get());
    }
}

// static pg::fVec2 getPointInCircle(double radius)
// {
//     auto a = randomBetween(0.0, 1.0);
//     auto b = randomBetween(0.0, 1.0);
//     if (b < a) { std::swap(a, b); }
//
//     return {static_cast<float>(b * radius * std::cos(2.0f * std::numbers::pi * a / b)),
//             static_cast<float>(b * radius * std::sin(2.0f * std::numbers::pi * a / b))};
// }

} // namespace pg