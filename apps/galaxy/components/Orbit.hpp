#pragma once
#include <entt/entt.hpp>

namespace galaxy {
struct Orbiter
{
    double epoch{}; //> epoch as fraction of the orbital period of the referenced orbit
    double mass{};
};
} // namespace galaxy