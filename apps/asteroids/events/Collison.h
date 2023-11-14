#pragma once
#include <SDLVec.h>

namespace game { namespace events {

struct Collision
{
    entt::entity c1;
    entt::entity c2;
};
}} // namespace game::events