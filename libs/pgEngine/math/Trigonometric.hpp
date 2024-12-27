#pragma once
#include <numbers>
#include <cmath>
#include <pgEngine/math/Vec.hpp>

namespace pg::math {

template <typename T>
using Polar = std::pair<T, T>;

template <typename T>
using Cartesian = Vec2<T>;

// TODO: strongly typed degrees and radians
template <typename T>
using Degrees = T;

template <typename T>
using Radians = T;

// radians to degrees
template <typename T>
constexpr auto toDegrees(Radians<T> radians) -> Degrees<T>
{
    return Degrees<T>{radians * static_cast<T>(180.0 / std::numbers::pi)};
}

// degrees to radians
template <typename T>
constexpr auto toRadians(Degrees<T> degrees) -> Radians<T>
{
    return Radians<T>{degrees * static_cast<T>(std::numbers::pi / 180.0)};
}

// normalize angle to 0-360
template <typename T>
constexpr auto normalizeAngle(Degrees<T> angle) -> Degrees<T>
{
    return std::fmod(angle + static_cast<T>(360.0), static_cast<T>(360.0));
}

// Cartesian to polar
template <typename T>
constexpr auto toPolar(const Cartesian<T>& v) -> Polar<T>
{
    return {std::sqrt(v[0] * v[0] + v[1] * v[1]), std::atan2(v[1], v[0])};
}

} // namespace pg::math