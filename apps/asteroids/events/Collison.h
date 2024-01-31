#pragma once
#include <SDLVec.h>

namespace asteroids { namespace events {

struct Collision
{
    entt::entity c1;
    entt::entity c2;
};
}} // namespace asteroids::events