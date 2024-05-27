#pragma once
#include <deque>
#include <entt/entt.hpp>
#include <memory>

namespace behavior::port {

using TargetSystems = std::vector<entt::entity>;
}

namespace BT {
template <>
inline std::vector<entt::entity> convertFromString(StringView str)
{
    // We expect real numbers separated by semicolons
    auto                      parts = splitString(str, ';');
    std::vector<entt::entity> output;
    for (auto& part : parts)
    {
        output.push_back(convertFromString<entt::entity>(part));
    }

    return output;
}
} // end namespace BT
