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
    float radius;
};

static bool intersects(const BoundingSphere  sphere1,
                       const Transform&      t1,
                       const BoundingSphere& sphere2,
                       const Transform&      t2)
{
    const auto max_scale_t1 = std::max(t1.scale[0], t1.scale[1]);
    const auto max_scale_t2 = std::max(t2.scale[0], t2.scale[1]);
    const auto diff = t1.pos - t2.pos;
    const auto distSquared = dot(diff, diff);
    const auto radiiSum = sphere1.radius * max_scale_t1 + sphere2.radius * max_scale_t2;
    return distSquared < radiiSum * radiiSum;
}

} // namespace pg