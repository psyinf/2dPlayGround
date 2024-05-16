#pragma once
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <ostream>
#include <ranges>
#include <type_traits>
#include <limits>

namespace pg {

template <typename ELEMENT_TYPE, size_t SIZE>
using Vec = std::array<ELEMENT_TYPE, SIZE>;

template <typename ELEMENT_TYPE>
using Vec2 = Vec<ELEMENT_TYPE, 2>;

template <typename ELEMENT_TYPE>
using Vec3 = Vec<ELEMENT_TYPE, 3>;

template <typename ELEMENT_TYPE>
using Vec4 = Vec<ELEMENT_TYPE, 4>;

using Vec2Window = Vec2<int32_t>;
using iVec2 = Vec2Window;
using fVec2 = Vec2<float>;
using Color = Vec4<uint8_t>;

} // namespace pg

template <typename T, size_t SIZE>
static constexpr std::ostream& operator<<(std::ostream& os, const pg::Vec<T, SIZE>& rhs)
{
    os << "[";
    for (auto idx : std::views::iota(size_t{}, rhs.size()))
    {
        os << rhs[idx];
        if (idx != rhs.size() - 1) { os << ", "; }
    }
    os << "]";
    return os;
}

template <typename T, size_t SIZE, typename Functor>
auto elementWise(const Functor& f, const pg::Vec<T, SIZE>& rhs)
{
    pg::Vec<T, SIZE> res;
    for (auto idx : std::views::iota(size_t{}, res.size()))
    {
        res[idx] = f(rhs[idx]);
    }
    return res;
}

template <typename T, size_t SIZE>
constexpr pg::Vec<T, SIZE> operator+(const pg::Vec<T, SIZE>& lhs, const pg::Vec<T, SIZE>& rhs)
{
    pg::Vec<T, SIZE> res;
    for (auto idx : std::views::iota(size_t{}, lhs.size()))
    {
        res[idx] = lhs[idx] + rhs[idx];
    }
    return res;
}

template <typename T, size_t SIZE>
constexpr auto operator+=(pg::Vec<T, SIZE>& lhs, const pg::Vec<T, SIZE>& rhs)
{
    for (auto idx : std::views::iota(size_t{}, lhs.size()))
    {
        lhs[idx] += rhs[idx];
    }
    return lhs;
}

template <typename T, size_t SIZE>
constexpr auto operator-=(pg::Vec<T, SIZE>& lhs, const pg::Vec<T, SIZE>& rhs)
{
    for (auto idx : std::views::iota(size_t{}, lhs.size()))
    {
        lhs[idx] -= rhs[idx];
    }
    return lhs;
}

template <typename T, size_t SIZE>
constexpr auto operator*=(pg::Vec<T, SIZE>& lhs, const pg::Vec<T, SIZE>& rhs)
{
    for (auto idx : std::views::iota(size_t{}, lhs.size()))
    {
        lhs[idx] *= rhs[idx];
    }
    return lhs;
}

template <typename T, size_t SIZE>
static constexpr pg::Vec<T, SIZE> operator-(const pg::Vec<T, SIZE>& lhs, const pg::Vec<T, SIZE>& rhs)
{
    pg::Vec<T, SIZE> res;
    for (auto idx : std::views::iota(size_t{}, lhs.size()))
    {
        res[idx] = lhs[idx] - rhs[idx];
    }
    return res;
}

template <typename T, size_t SIZE>
static constexpr pg::Vec<T, SIZE> operator*(const pg::Vec<T, SIZE>& lhs, const T& rhs)
{
    pg::Vec<T, SIZE> res;
    for (auto idx : std::views::iota(size_t{}, lhs.size()))
    {
        res[idx] = lhs[idx] * rhs;
    }
    return res;
}

template <typename T, size_t SIZE>
static constexpr pg::Vec<T, SIZE> operator*(const T& lhs, const pg::Vec<T, SIZE>& rhs)
{
    pg::Vec<T, SIZE> res;
    for (auto idx : std::views::iota(size_t{}, rhs.size()))
    {
        res[idx] = rhs[idx] * lhs;
    }
    return res;
}

template <typename T, size_t SIZE>
static constexpr pg::Vec<T, SIZE> operator/(const pg::Vec<T, SIZE>& lhs, const T& rhs)
{
    pg::Vec<T, SIZE> res;
    for (auto idx : std::views::iota(size_t{}, lhs.size()))
    {
        res[idx] = lhs[idx] / rhs;
    }
    return res;
}

template <typename T, size_t SIZE>
static constexpr pg::Vec<T, SIZE> operator/(const T& lhs, const pg::Vec<T, SIZE>& rhs)
{
    pg::Vec<T, SIZE> res;
    for (auto idx : std::views::iota(size_t{}, rhs.size()))
    {
        res[idx] = lhs / rhs[idx];
    }
    return res;
}

template <typename T, size_t SIZE>
static constexpr void operator*=(pg::Vec<T, SIZE>& lhs, const T& rhs)
{
    for (auto idx : std::views::iota(size_t{}, lhs.size()))
    {
        lhs[idx] *= rhs;
    }
}

template <typename T, size_t SIZE>
static constexpr void operator*=(const T& lhs, pg::Vec<T, SIZE>& rhs)
{
    for (auto idx : std::views::iota(size_t{}, rhs.size()))
    {
        rhs[idx] *= lhs;
    }
}

template <typename T, size_t SIZE>
static constexpr void operator/=(pg::Vec<T, SIZE>& lhs, const T& rhs)
{
    for (auto idx : std::views::iota(size_t{}, lhs.size()))
    {
        lhs[idx] /= rhs;
    }
}

template <typename T, size_t SIZE>
static constexpr void operator/=(const T& lhs, pg::Vec<T, SIZE>& rhs)
{
    for (auto idx : std::views::iota(size_t{}, rhs.size()))
    {
        rhs[idx] = lhs / rhs[idx];
    }
}

template <typename T, size_t SIZE>
constexpr auto operator*(const pg::Vec<T, SIZE>& lhs, const pg::Vec<T, SIZE>& rhs)
{
    pg::Vec<T, SIZE> res;
    for (auto idx : std::views::iota(size_t{}, lhs.size()))
    {
        res[idx] = lhs[idx] * rhs[idx];
    }
    return res;
}

template <typename T, size_t SIZE>
static constexpr T dot(const pg::Vec<T, SIZE>& lhs, const pg::Vec<T, SIZE>& rhs)
{
    T res{};
    for (auto idx : std::views::iota(size_t{}, lhs.size()))
    {
        res += lhs[idx] * rhs[idx];
    }
    return res;
}

template <typename T>
static constexpr T det(const pg::Vec<T, 2>& lhs, const pg::Vec<T, 2>& rhs)
{
    return lhs[0] * rhs[1] - lhs[1] * rhs[0];
}

template <typename T>
static constexpr T angleBetween(const pg::Vec<T, 2>& lhs, const pg::Vec<T, 2>& rhs)
{
    auto dot_product = dot(lhs, rhs);
    auto determinant = det(lhs, rhs);
    return atan2(determinant, dot_product);
}

template <typename T, size_t SIZE>
static constexpr T lengthSquared(const pg::Vec<T, SIZE>& lhs)
{
    T res{};
    for (auto idx : std::views::iota(size_t{}, lhs.size()))
    {
        res += lhs[idx] * lhs[idx];
    }
    return res;
}

template <typename T, size_t SIZE>
static constexpr T length(const pg::Vec<T, SIZE>& lhs)
{
    T res{};
    for (auto idx : std::views::iota(size_t{}, lhs.size()))
    {
        res += lhs[idx] * lhs[idx];
    }
    return std::sqrt(res);
}

template <typename T, size_t SIZE>
static constexpr T normalize(pg::Vec<T, SIZE>& lhs)
{
    auto l = length(lhs);
    for (auto idx : std::views::iota(size_t{}, lhs.size()))
    {
        lhs[idx] /= l;
    }
    return l;
}

template <typename T, size_t SIZE>
static constexpr pg::Vec<T, SIZE> makeNormal(const pg::Vec<T, SIZE>& lhs)
{
    auto copy = lhs;
    normalize(copy);
    return copy;
}

template <typename CAST_T, typename T, size_t SIZE>
static constexpr pg::Vec<CAST_T, SIZE> vec_cast(const pg::Vec<T, SIZE>& lhs)
{
    pg::Vec<CAST_T, SIZE> res;
    for (auto idx : std::views::iota(size_t{}, lhs.size()))
    {
        res[idx] = static_cast<CAST_T>(lhs[idx]);
    }
    return res;
}

template <typename T, size_t SIZE>
static constexpr bool equal(const pg::Vec<T, SIZE>& lhs,
                            const pg::Vec<T, SIZE>& rhs,
                            T                       epsilon = std::numeric_limits<T>::epsilon())
{
    for (auto idx : std::views::iota(size_t{}, lhs.size()))
    {
        if (std::abs(lhs[idx] - rhs[idx]) > epsilon) { return false; }
    }
    return true;
}
