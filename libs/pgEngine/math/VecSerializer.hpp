#pragma once
#include <pgEngine/math/VecOps.hpp>

#include <nlohmann/json.hpp>

namespace pg {
template <typename T, size_t N>
constexpr inline void to_json(nlohmann::json& j, const Vec<T, N>& v)
{
    j = nlohmann::json::array();
    for (size_t i = 0; i < N; i++)
    {
        j.push_back(v[i]);
    }
}

template <typename T, size_t N>
constexpr inline void from_json(const nlohmann::json& j, const Vec<T, N>& p)
{
    for (size_t i = 0; i < N; i++)
    {
        p[i] = j[i];
    }
}
} // namespace pg