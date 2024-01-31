#pragma once
#include <SDLVec.h>
namespace asteroids { namespace events {

struct LaserFired
{
    pg::fVec2 offset;
    entt::entity shooter;
};
}}