#pragma once
#include <pgEngine/math/Vec.hpp>

namespace pg {
enum class TransformScaleSpace
{
    Local, // global scale not applied
    World  // global scale applied
};

template <typename T>
struct GenTransform2D
{
    Vec2<T>             pos{};
    T                   rotation_deg{};
    Vec2<T>             scale{1, 1};
    TransformScaleSpace scaleSpace{TransformScaleSpace::World};

    friend GenTransform2D<T> operator+(const GenTransform2D<T>& lhs, const GenTransform2D<T>& rhs)
    {
        return {lhs.pos + rhs.pos, lhs.rotation_deg + rhs.rotation_deg, lhs.scale};
    }

    template <typename U>
    friend GenTransform2D<U> operator-(const GenTransform2D<T>& lhs, const GenTransform2D<U>& rhs)
    {
        return {lhs.pos - rhs.pos, lhs.rotation_deg - rhs.rotation_deg, lhs.scale};
    }
};

using Transform2D = GenTransform2D<float>;
using PrecTransform2D = GenTransform2D<double>;

} // namespace pg
