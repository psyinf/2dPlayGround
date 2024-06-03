#pragma once
#include <pgEngine/math/Bounds.hpp>
#include <pgEngine/math/Transform.hpp>
#include <optional>

namespace pg {

static auto intersects(const BoundingSphere  sphere1,
                       const Transform2D&    t1,
                       const BoundingSphere& sphere2,
                       const Transform2D&    t2)
{
    const auto max_scale_t1 = std::max(t1.scale[0], t1.scale[1]);
    const auto max_scale_t2 = std::max(t2.scale[0], t2.scale[1]);
    const auto diff = t1.pos - t2.pos;
    const auto distSquared = pg::dot(diff, diff);
    const auto radiiSum = sphere1.radius * max_scale_t1 + sphere2.radius * max_scale_t2;
    auto       intrusion = distSquared - (radiiSum * radiiSum);
    return intrusion < 0.0f ? std::optional<float>{intrusion} : std::nullopt;
}

} // namespace pg