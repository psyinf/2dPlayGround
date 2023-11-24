#pragma once
#include "SDLVec.h"

namespace pg {
struct Transform
{
    fVec2 pos{};
    float rotation_deg{};
    fVec2 scale{1, 1};
};

struct BoundingSphere
{
    static float fromRectangle(const pg::iVec2& rect) { return 0.5 * std::sqrt(dot(rect, rect)); }

    float radius;
};


} // namespace pg