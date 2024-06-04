#pragma once
#include <pgEngine/math/VecOps.hpp>

namespace pg {

struct BoundingSphere
{
    static float fromRectangle(const pg::iVec2& rect)
    {
        return 0.5f * std::sqrtf(pg::dot(pg::vec_cast<float>(rect), pg::vec_cast<float>(rect)));
    }

    float radius;
};

} // namespace pg