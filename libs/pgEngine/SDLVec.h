#pragma once
#include <algorithm>
#include <array>
#include <cstdint>
#include <ranges>

namespace pg {
template <typename ELEMENT_TYPE>
using Vec2 = std::array<ELEMENT_TYPE, 2>;

template <typename ELEMENT_TYPE>
using Vec3 = std::array<ELEMENT_TYPE, 2>;

using Vec2Window = Vec2<int32_t>;
using iVec2 = Vec2Window;
using fVec2 = Vec2<float>;

} // namespace pg

template <class T>
struct is_std_array : std::false_type
{
};

template <class T, size_t N>
struct is_std_array<std::array<T, N>> : std::true_type
{
};

template <class T>
constexpr bool is_std_array_v = is_std_array<std::remove_cvref_t<T>>::value;

template <typename T>
pg::Vec2<T> operator+(const pg::Vec2<T>& lhs, const pg::Vec2<T>& rhs)
{
    // for now
    pg::Vec2<T> res;
    for (auto idx : std::views::iota(0u, lhs.size()))
    {
        res[idx] = lhs[idx] + rhs[idx];
    }
    return res;
}

template <typename T>
pg::Vec2<T> operator-(const pg::Vec2<T>& lhs, const pg::Vec2<T>& rhs)
{
    // for now
    pg::Vec2<T> res;
    for (auto idx : std::views::iota(0u, lhs.size()))
    {
        res[idx] = lhs[idx] - rhs[idx];
    }
    return res;
}

template <typename T>
T dot(const pg::Vec2<T>& lhs, const pg::Vec2<T>& rhs)
{
    T res{};
    for (auto idx : std::views::iota(0u, lhs.size()))
    {
        res += lhs[idx] * rhs[idx];
    }
    return res;
}