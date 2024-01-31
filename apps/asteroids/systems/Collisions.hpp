#pragma once
#include <entt/entt.hpp>
#include <events/LaserFired.h>
#include <systems/SystemInterface.hpp>

namespace asteroids {
using entt::literals::operator""_hs;

class Collisions : public pg::game::SystemInterface
{
public:
    using SystemInterface::SystemInterface;

    void setup();

    void handle(const pg::game::FrameStamp& frameStamp);

private:
    void handleCollision(entt::entity id1, entt::entity id2, float intrusion);
};

} // namespace asteroids