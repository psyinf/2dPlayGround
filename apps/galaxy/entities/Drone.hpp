#pragma once
#include <pgEngine/math/Vec.hpp>

namespace galaxy {

struct Drone
{
    uint16_t     lifetime{0};
    bool         hasTarget{false};
    bool         atTarget{false};
    entt::entity targetId;
    uint16_t     waitCycles{100};
    pg::fVec2    targetPos{};
};

} // namespace galaxy
