#pragma once
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
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
using Color = Vec4<uint8_t>;

} // namespace pg

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
constexpr auto operator*=(pg::Vec<T, SIZE>& lhs, const pg::Vec<T, SIZE>& rhs)
{
    for (auto idx : std::views::iota(size_t{}, lhs.size()))
    {
        lhs[idx] *= rhs[idx];
    }
    return lhs;
}

template <typename T, size_t SIZE>
pg::Vec<T, SIZE> operator-(const pg::Vec<T, SIZE>& lhs, const pg::Vec<T, SIZE>& rhs)
{
    pg::Vec<T, SIZE> res;
    for (auto idx : std::views::iota(size_t{}, lhs.size()))
    {
        res[idx] = lhs[idx] - rhs[idx];
    }
    return res;
}

template <typename T, size_t SIZE>
pg::Vec<T, SIZE> operator*(const pg::Vec<T, SIZE>& lhs, const T& rhs)
{
    pg::Vec<T, SIZE> res;
    for (auto idx : std::views::iota(size_t{}, lhs.size()))
    {
        res[idx] = lhs[idx] * rhs;
    }
    return res;
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
T dot(const pg::Vec<T, SIZE>& lhs, const pg::Vec<T, SIZE>& rhs)
{
    T res{};
    for (auto idx : std::views::iota(size_t{}, lhs.size()))
    {
        res += lhs[idx] * rhs[idx];
    }
    return res;
}

template <typename T, size_t SIZE>
T lengthSquared(const pg::Vec<T, SIZE>& lhs)
{
    T res{};
    for (auto idx : std::views::iota(size_t{}, lhs.size()))
    {
        res += lhs[idx] * lhs[idx];
    }
    return res;
}

template <typename T, size_t SIZE>
T length(const pg::Vec<T, SIZE>& lhs)
{
    T res{};
    for (auto idx : std::views::iota(size_t{}, lhs.size()))
    {
        res += lhs[idx] * lhs[idx];
    }
    return std::sqrt(res);
}

template <typename T, size_t SIZE>
T normalize(pg::Vec<T, SIZE>& lhs)
{
    auto l = length(lhs);
    for (auto idx : std::views::iota(size_t{}, lhs.size()))
    {
        lhs[idx] /= l;
    }
    return l;
}

template <typename T, size_t SIZE>
pg::Vec<T, SIZE> makeNormal(const pg::Vec<T, SIZE>& lhs)
{
    auto copy = lhs;
    normalize(copy);
    return copy;
}

template <typename CAST_T, typename T, size_t SIZE>
pg::Vec<CAST_T, SIZE> vec_cast(const pg::Vec<T, SIZE>& lhs)
{
    pg::Vec<CAST_T, SIZE> res;
    for (auto idx : std::views::iota(size_t{}, lhs.size()))
    {
        res[idx] = static_cast<CAST_T>(lhs[idx]);
    }
    return res;
}

