#pragma once
#include <pgEngine/math/Vec.hpp>

namespace galaxy {
struct Faction
{
    std::string name;
    pg::Color   systemColor;
    pg::Color   entityColor;
    bool        started{false};
};
} // namespace galaxy