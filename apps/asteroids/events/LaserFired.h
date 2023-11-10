#pragma once
#include <SDLVec.h>
namespace game { namespace events {

struct LaserFired
{
    pg::fVec2 offset;
    entt::entity shooter;
};
}}