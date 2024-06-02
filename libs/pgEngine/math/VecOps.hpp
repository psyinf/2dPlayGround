#pragma once
#include <pgEngine/math/Vec.hpp>

namespace pg {
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
} // namespace pg