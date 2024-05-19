#pragma once
#include <pgEngine/math/Vec.hpp>

#include <Config.hpp>

namespace galaxy {

struct Dynamic
{
    pg::dVec2 velocity{};     // percent light speed
    pg::dVec2 acceleration{}; // m/s^2

    void reset() { velocity = acceleration = {}; }

    float calculateBreakinDistance(double maxAcceleration) const
    {
        return (lengthSquared(velocity)) / (2 * maxAcceleration);
    }

    pg::fVec2 calculateDynamics(const pg::fVec2& currentPos,
                                const pg::fVec2& targetPos,
                                double           maxAcceleration,
                                double           maxVelocity,
                                double           dt)
    {
        // breaking distance
        auto breakingDistance = calculateBreakinDistance(maxAcceleration);
        auto dir = vec_cast<double>(targetPos - currentPos);
        auto dist = normalize(dir);

        if (dist <= breakingDistance) { acceleration = -maxAcceleration * dir; }
        else { acceleration = maxAcceleration * dir; }
        // adapt velocity
        velocity += acceleration * dt;
        // limit velocity
        if (length(velocity) > maxVelocity) { velocity = makeNormal(velocity) * maxVelocity; }

        return vec_cast<float>(vec_cast<double>(currentPos) + velocity * dt);
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
