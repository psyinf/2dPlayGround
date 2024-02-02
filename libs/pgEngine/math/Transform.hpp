#pragma once
#include <pgEngine/math/Vec.hpp>

namespace pg {
struct Transform2D
{
    fVec2 pos{};
    float rotation_deg{};
    fVec2 scale{1, 1};
};
} // namespace pg