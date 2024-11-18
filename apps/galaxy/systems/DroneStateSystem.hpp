#pragma once
#include <entt/entt.hpp>
#include <pgGame/systems/SystemInterface.hpp>
#include <pgEngine/math/Vec.hpp>

#include <events/DroneEvents.hpp>
#include <Config.hpp>

namespace galaxy {
using entt::literals::operator""_hs;

class Game;

class DroneStateSystem : public pg::game::SystemInterface
{
public:
    using SystemInterface::SystemInterface;

    void setup(std::string_view scene_id) override;

    void handle(const pg::game::FrameStamp& frameStamp) override;

private:
};
} // namespace galaxy