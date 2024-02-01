#pragma once
#include <math/Vec.hpp>
#include <SDLPrimitives.h>
#include <memory>
#include <entt/entt.hpp>
namespace asteroids {
using entt::literals::operator""_hs;

//active colliders can collide with active and passive colliders
using ActiveCollider = entt::tag<"ActiveCollider"_hs>;
//passive colliders cannot collide with other active colliders
using PassiveCollider = entt::tag<"PassiveCollider"_hs>;
struct Drawable
{
    std::shared_ptr<pg::Primitive> prim;
};

struct Dynamics
{
    pg::fVec2 velocity{};
    pg::fVec2 dampening{1.0f, 1.0f};
};

struct HitPoints
{
    std::uint16_t remaining{};
};

struct Damage
{
    std::uint16_t amount{};
};



struct RenderConfig
{
    bool renderBroadPhaseCollisionShapes{};
};
} // namespace asteroids
