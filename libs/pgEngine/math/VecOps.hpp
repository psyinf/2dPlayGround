#pragma once
#include <pgEngine/math/Vec.hpp>
#include <ostream>

namespace pg {

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
constexpr auto elementWise(const Functor& f, const pg::Vec<T, SIZE>& rhs)
{
    pg::Vec<T, SIZE> res;
    for (auto idx : std::views::iota(size_t{}, res.size()))
    {
        res[idx] = f(rhs[idx]);
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
    auto l = pg::length(lhs);
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
    pg::Vec<CAST_T, SIZE> res{};
    for (auto idx : std::views::iota(size_t{}, SIZE))
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

template <typename T, size_t SIZE>
static constexpr pg::Vec<T, SIZE> clampBetween(const pg::Vec<T, SIZE>& lhs,
                                               const pg::Vec<T, SIZE>& min,
                                               const pg::Vec<T, SIZE>& max)
{
    pg::Vec<T, SIZE> res;
    for (auto idx : std::views::iota(size_t{}, lhs.size()))
    {
        res[idx] = std::clamp(lhs[idx], min[idx], max[idx]);
    }
    return res;
}

template <typename T, size_t SIZE>
static constexpr T distance(const pg::Vec<T, SIZE>& lhs, const pg::Vec<T, SIZE>& rhs)
{
    return pg::length(lhs - rhs);
}

template <typename T, typename U, size_t SIZE>
static constexpr pg::Vec<T, SIZE> scale(const pg::Vec<T, SIZE>& lhs, U scale)
{
    pg::Vec<T, SIZE> res{};
    for (auto idx : std::views::iota(size_t{}, lhs.size()))
    {
        res[idx] = T(lhs[idx] * scale);
    }
    return res;
}

constexpr pg::Color asRBGA(const std::array<uint8_t, 3>& rgb, uint8_t alpha = 255)
{
    return {rgb[0], rgb[1], rgb[2], alpha};
}

template <typename>
struct VecTraits;

template <template <typename, size_t> class VecType, typename T, size_t SIZE>
struct VecTraits<VecType<T, SIZE>>
{
    using ValueType = T;
    static constexpr size_t Capacity = SIZE;
};

template <size_t... Indices>
struct Swizzle
{
};

// for now: we could pre-define the Indices as xyz etc but we need to pass std::integer_sequence<size_t, Indices...>
template <size_t... Indices, typename Vec>
auto sub(const Vec& lhs) -> pg::Vec<typename VecTraits<Vec>::ValueType, sizeof...(Indices)>
{
    constexpr size_t VecCapacity = VecTraits<Vec>::Capacity;
    using ContainedType = typename VecTraits<Vec>::ValueType;
    // check if the indices are within the bounds of the vector
    static_assert(((Indices < VecCapacity) && ...), "Index out of bounds");

    pg::Vec<ContainedType, sizeof...(Indices)> res{};
    size_t                                     idx = 0;
    ((res[idx++] = lhs[Indices]), ...);
    return res;
}

template <size_t... Indices, typename Vec>
auto swizzle(const Vec& lhs, Swizzle<Indices...>) -> pg::Vec<typename VecTraits<Vec>::ValueType, sizeof...(Indices)>
{
    constexpr size_t VecCapacity = VecTraits<Vec>::Capacity;
    using ContainedType = typename VecTraits<Vec>::ValueType;
    // check if the indices are within the bounds of the vector
    static_assert(((Indices < VecCapacity) && ...), "Index out of bounds");

    pg::Vec<ContainedType, sizeof...(Indices)> res{};
    size_t                                     idx = 0;
    ((res[idx++] = lhs[Indices]), ...);
    return res;
}

using XYZ = Swizzle<0, 1, 2>;
using RGB = Swizzle<0, 1, 2>;
using RGBA = Swizzle<0, 1, 2, 3>;
using XY = Swizzle<0, 1>;

} // namespace pg
