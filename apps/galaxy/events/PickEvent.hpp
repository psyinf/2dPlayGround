#pragma once
#include <pgEngine/math/Vec.hpp>

namespace galaxy { namespace events {

struct PickEvent
{
    pg::iVec2 screen_position;
    pg::fVec2 world_position;
    // MouseButton button;
};
}} // namespace galaxy::events