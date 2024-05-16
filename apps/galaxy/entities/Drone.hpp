#pragma once
#include <pgEngine/math/Vec.hpp>

#include <Config.hpp>

namespace galaxy {

struct Drone
{
    static Drone fromConfig(const galaxy::config::DroneParameters& config)
    {
        Drone drone;
        drone.range = config.max_range;
        drone.maxAcceleration = config.max_acceleration;
        drone.maxVelocity = config.max_speed;

        return drone;
    }

    uint16_t lifetime{0};
    bool     hasTarget{false};
    bool     atTarget{false};

    entt::entity targetId;
    uint16_t     waitCycles{100};
    pg::fVec2    targetPos{};

    float range{100};
    float maxAcceleration{0.0001f};
    float maxVelocity{};
};

} // namespace galaxy
