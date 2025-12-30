#pragma once
#include <pgEngine/math/Vec.hpp>
#include <entt/entt.hpp>

#include <pgFoundation/NamedTypeRegistrar.hpp>

namespace asteroids { namespace events {

struct LaserFired : pgf::TypeRegistrar<LaserFired, "LaserFired">
{
    pg::fVec2    offset;
    entt::entity shooter;
};
}} // namespace asteroids::events
