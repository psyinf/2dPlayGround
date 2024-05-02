#pragma once
#include <array>
#include <pgEngine/math/Vec.hpp>

namespace pg {
template <typename T>
class Circle
{
public:
    Vec2<T> pos;
    T       radius;

    constexpr Circle(const Vec2<T>& offset, T& radius) noexcept
      : pos{offset}
      , radius{radius}
    {
    }

    // TODO: check with integer types
    constexpr decltype(auto) midpoint() const noexcept { return pos }

    constexpr decltype(auto) size() const noexcept { return radius; }

    constexpr bool intersects(const Box<T>& box) const noexcept
    {
        auto halfWidth = box.width() / 2;
        auto halfHeight = box.height() / 2;
        auto distX = abs(midpoint()[0] - box.midpoint()[0]);
        auto distY = abs(midpoint()[1] - box.midpoint()[1]);

        if (distX > (halfWidth + radius)) { return false; }
        if (distY > (halfHeight + radius)) { return false; }

        if (distX <= halfWidth) { return true; }
        if (distY <= halfHeight) { return true; }

        auto dx = distX - halfWidth;
        auto dy = distY - halfHeight;
        return (dx * dx + dy * dy <= radius * radius);
    }
};

using fBox = Box<float>;
} // namespace pg