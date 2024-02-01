#pragma once
#include <pgEngine/math/Vec.hpp>
namespace asteroids { namespace events {

struct LaserFired
{
    pg::fVec2 offset;
    entt::entity shooter;
};
}} // namespace asteroids::events