#pragma once
#include <entt/entt.hpp>
#include <pgGame/systems/SystemInterface.hpp>

namespace asteroids {
using entt::literals::operator""_hs;

class Collisions : public pg::game::SystemInterface
{
public:
    using SystemInterface::SystemInterface;

    void setup(std::string_view scene_id);

    void handle(const pg::FrameStamp& frameStamp);

private:
    void handleCollision(entt::entity id1, entt::entity id2, float intrusion);
};

} // namespace asteroids
