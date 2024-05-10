#pragma once
#include <pgEngine/math/Vec.hpp>

namespace pg {
enum class TransformScaleSpace
{
    Local, // global scale not applied
    World  // global scale applied
};

struct Transform2D
{
    fVec2               pos{};
    float               rotation_deg{};
    fVec2               scale{1, 1};
    TransformScaleSpace scaleSpace{TransformScaleSpace::World};

    friend Transform2D operator+(const Transform2D& lhs, const Transform2D& rhs)
    {
        return {lhs.pos + rhs.pos, lhs.rotation_deg + rhs.rotation_deg, lhs.scale};
    }
};

} // namespace pg