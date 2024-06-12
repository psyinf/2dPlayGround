#pragma once
#include <pgEngine/math/Vec.hpp>

namespace galaxy { namespace events {

struct PickEvent
{
    pg::iVec2 screen_position;
    pg::fVec2 world_position;
    pg::fVec2 scale;
    float     radius{0};
    // MouseButton button;
};

struct PickResult
{
    pg::fVec2    world_position;
    entt::entity entity;
};
}} // namespace galaxy::events