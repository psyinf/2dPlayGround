#pragma once
#include <array>
#include <pgEngine/math/Vec.hpp>

// based on https://github.com/pvigier/Quadtree/blob/master/include/Box.h
namespace pg {
template <typename T>
class Box
{
public:
    Vec2<T> pos;
    Vec2<T> dim;

    constexpr Box(const Vec2<T>& offset, const Vec2<T>& dims) noexcept
      : pos{offset}
      , dim{dims}
    {
    }

    constexpr decltype(auto) getMin() const noexcept { return pos; }

    constexpr decltype(auto) getMax() const noexcept { return pos + dim; }

    constexpr decltype(auto) left() const noexcept { return pos[0]; }

    constexpr decltype(auto) right() const noexcept { return pos[0] + dim[0]; }

    constexpr decltype(auto) top() const noexcept { return pos[1]; }

    constexpr decltype(auto) bottom() const noexcept { return pos[1] + dim[1]; }

    constexpr decltype(auto) width() const noexcept { return dim[0]; }

    constexpr decltype(auto) height() const noexcept { return dim[1]; }

    // TODO: check with integer types
    constexpr decltype(auto) midpoint() const noexcept { return pos + dim * static_cast<T>(0.5); }

    constexpr decltype(auto) size() const noexcept { return dim; }

    constexpr bool contains(const Box<T>& box) const noexcept
    {
        // Check if the other box is within the current box
        return this->left() <= box.left() && this->right() >= box.right() && this->top() <= box.top() &&
               this->bottom() >= box.bottom();
    }

    constexpr bool contains(const Vec2<T>& point) const noexcept
    {
        return this->left() <= point[0] && this->right() >= point[0] && this->top() <= point[1] &&
               this->bottom() >= point[1];
    }

    constexpr bool intersects(const Box<T>& box) const noexcept
    {
        // Check if box is to the right
        if (this->left() >= box.right()) return false;
        // Check if box is to the left
        if (this->right() <= box.left()) return false;
        // Check if box is above
        if (this->bottom() <= box.top()) return false;
        // Check if box is below
        if (this->top() >= box.bottom()) return false;
        return true;
    }
};

using fBox = Box<float>;
} // namespace pg