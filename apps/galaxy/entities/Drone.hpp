#pragma once
#include <pgEngine/math/Vec.hpp>

namespace galaxy {

struct Drone
{
    uint8_t      id;
    bool         isAlive{true};
    bool         hasTarget{false};
    bool         atTarget{false};
    entt::entity targetId;
    uint16_t     waitCycles{100};
    pg::fVec2    targetPos{};
};

} // namespace galaxy
