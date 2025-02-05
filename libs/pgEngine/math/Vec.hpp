#pragma once
#include <array>
#include <cstdint>
#include <ranges>

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
using dVec2 = Vec2<double>;
using Color = Vec4<uint8_t>;
using ColorRBG = Vec3<uint8_t>;

} // namespace pg

template <typename T, size_t SIZE>
constexpr pg::Vec<T, SIZE> operator+(const pg::Vec<T, SIZE>& lhs, const pg::Vec<T, SIZE>& rhs)
{
    pg::Vec<T, SIZE> res{};
    for (auto idx : std::views::iota(size_t{}, SIZE))
    {
        res[idx] = lhs[idx] + rhs[idx];
    }
    return res;
}

template <typename T, size_t SIZE>
constexpr auto operator+=(pg::Vec<T, SIZE>& lhs, const pg::Vec<T, SIZE>& rhs)
{
    for (auto idx : std::views::iota(size_t{}, SIZE))
    {
        lhs[idx] += rhs[idx];
    }
    return lhs;
}

template <typename T, size_t SIZE>
constexpr auto operator-=(pg::Vec<T, SIZE>& lhs, const pg::Vec<T, SIZE>& rhs)
{
    for (auto idx : std::views::iota(size_t{}, SIZE))
    {
        lhs[idx] -= rhs[idx];
    }
    return lhs;
}

template <typename T, size_t SIZE>
constexpr auto operator*=(pg::Vec<T, SIZE>& lhs, const pg::Vec<T, SIZE>& rhs)
{
    for (auto idx : std::views::iota(size_t{}, SIZE))
    {
        lhs[idx] *= rhs[idx];
    }
    return lhs;
}

template <typename T, size_t SIZE>
static constexpr pg::Vec<T, SIZE> operator-(const pg::Vec<T, SIZE>& lhs, const pg::Vec<T, SIZE>& rhs)
{
    pg::Vec<T, SIZE> res{};
    for (auto idx : std::views::iota(size_t{}, SIZE))
    {
        res[idx] = lhs[idx] - rhs[idx];
    }
    return res;
}

template <typename T, size_t SIZE>
static constexpr pg::Vec<T, SIZE> operator*(const pg::Vec<T, SIZE>& lhs, const T& rhs)
{
    pg::Vec<T, SIZE> res{};
    // static_assert((lhs.size() == SIZE));
    for (auto idx : std::views::iota(size_t{}, SIZE))
    {
        res[idx] = lhs[idx] * rhs;
    }
    return res;
}

template <typename T, size_t SIZE>
static constexpr pg::Vec<T, SIZE> operator*(const T& lhs, const pg::Vec<T, SIZE>& rhs)
{
    pg::Vec<T, SIZE> res{};
    for (auto idx : std::views::iota(size_t{}, SIZE))
    {
        res[idx] = rhs[idx] * lhs;
    }
    return res;
}

template <typename T, size_t SIZE>
static constexpr pg::Vec<T, SIZE> operator/(const pg::Vec<T, SIZE>& lhs, const T& rhs)
{
    pg::Vec<T, SIZE> res{};
    for (auto idx : std::views::iota(size_t{}, SIZE))
    {
        res[idx] = lhs[idx] / rhs;
    }
    return res;
}

template <typename T, size_t SIZE>
static constexpr pg::Vec<T, SIZE> operator/(const T& lhs, const pg::Vec<T, SIZE>& rhs)
{
    pg::Vec<T, SIZE> res{};
    for (auto idx : std::views::iota(size_t{}, SIZE))
    {
        res[idx] = lhs / rhs[idx];
    }
    return res;
}

template <typename T, size_t SIZE>
static constexpr void operator*=(pg::Vec<T, SIZE>& lhs, const T& rhs)
{
    for (auto idx : std::views::iota(size_t{}, SIZE))
    {
        lhs[idx] *= rhs;
    }
}

template <typename T, size_t SIZE>
static constexpr void operator*=(const T& lhs, pg::Vec<T, SIZE>& rhs)
{
    for (auto idx : std::views::iota(size_t{}, SIZE))
    {
        rhs[idx] *= lhs;
    }
}

template <typename T, size_t SIZE>
static constexpr void operator/=(pg::Vec<T, SIZE>& lhs, const T& rhs)
{
    for (auto idx : std::views::iota(size_t{}, SIZE))
    {
        lhs[idx] /= rhs;
    }
}

template <typename T, size_t SIZE>
static constexpr void operator/=(const T& lhs, pg::Vec<T, SIZE>& rhs)
{
    for (auto idx : std::views::iota(size_t{}, SIZE))
    {
        rhs[idx] = lhs / rhs[idx];
    }
}

template <typename T, size_t SIZE>
constexpr auto operator*(const pg::Vec<T, SIZE>& lhs, const pg::Vec<T, SIZE>& rhs)
{
    pg::Vec<T, SIZE> res{};
    for (auto idx : std::views::iota(size_t{}, SIZE))
    {
        res[idx] = lhs[idx] * rhs[idx];
    }
    return res;
}

// unary minus
template <typename T, size_t SIZE>
constexpr auto operator-(const pg::Vec<T, SIZE>& rhs)
{
    pg::Vec<T, SIZE> res{};
    for (auto idx : std::views::iota(size_t{}, SIZE))
    {
        res[idx] = -rhs[idx];
    }
    return res;
}
