#pragma once
#include <entt/entt.hpp>
#include <pgGame/systems/SystemInterface.hpp>

#include <events/DroneEvents.hpp>

namespace galaxy {
using entt::literals::operator""_hs;

class Game;

class DroneStateSystem : public pg::game::SystemInterface
{
public:
    using SystemInterface::SystemInterface;

    void setup(std::string_view scene_id) override;

    void handle(const pg::FrameStamp& frameStamp) override;

private:
};
} // namespace galaxy