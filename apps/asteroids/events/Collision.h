#pragma once
#include <pgEngine/math/Vec.hpp>
#include <pgFoundation/NamedTypeRegistrar.hpp>

namespace asteroids { namespace events {

struct Collision : public pgf::TypeRegistrar<Collision, "Collision">
{
    Collision(entt::entity c1, entt::entity c2)
      : c1(c1)
      , c2(c2)
    {
    }

    entt::entity c1;
    entt::entity c2;
};
}} // namespace asteroids::events
