#pragma once
#include <pgEngine/math/Vec.hpp>

namespace pg {

struct BoundingSphere
{
    static float fromRectangle(const pg::iVec2& rect) { return 0.5 * std::sqrt(dot(rect, rect)); }

    float radius;
};

} // namespace pg