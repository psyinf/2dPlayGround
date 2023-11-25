#pragma once
#include <SDLVec.h>
#include <SDLPrimitives.h>
#include <memory>
#include <entt/entt.hpp>
namespace game {
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
    pg::fVec2 dampening{0.9, 0.9};
};

struct HitPoints
{
    std::uint16_t remaining{};
};

struct Damage
{
    std::uint16_t amount{};
};

struct WindowDetails
{
    SDL_Rect windowRect;
};
} // namespace game
