#pragma once
#include <pgEngine/math/Vec.hpp>

#include <Config.hpp>

namespace galaxy {

struct Dynamic
{
    pg::fVec2 velocity{};     // percent light speed
    pg::fVec2 acceleration{}; // m/s^2

    void update(float dt) { velocity += acceleration * dt; }

    float calculateBreakinDistance(float maxAcceleration) const
    {
        return (lengthSquared(velocity)) / (2 * maxAcceleration);
    }

    void calculateDynamics(const pg::fVec2& currentPos,
                           const pg::fVec2& targetPos,
                           float            maxAcceleration,
                           float            maxVelocity,
                           float            dt)
    {
        // breaking distance
        auto breakingDistance = calculateBreakinDistance(maxAcceleration);
        auto dir = targetPos - currentPos;
        auto dist = normalize(dir);

        if (dist <= breakingDistance)
        {
            acceleration = -maxAcceleration * dir;
            return;
        }
        else { acceleration = maxAcceleration * dir; }
        // adapt velocity
        velocity += acceleration * dt;
        // limit velocity
        if (length(velocity) > maxVelocity) { velocity = makeNormal(velocity) * maxVelocity; }
    }
};

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
