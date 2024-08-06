#pragma once
#include <entt/entt.hpp>
#include <pgGame/systems/SystemInterface.hpp>
#include <pgEngine/math/Vec.hpp>

#include <events/DroneEvents.hpp>
#include <Config.hpp>
#include <behaviors/utils/Context.hpp>
#include <behaviortree_cpp/bt_factory.h>

namespace galaxy {
using entt::literals::operator""_hs;

class Game;
struct Drone;
struct Faction;

class DroneSystem : public pg::game::SystemInterface
{
public:
    using SystemInterface::SystemInterface;

    void setup();

    void handle(const pg::FrameStamp& frameStamp);

    void handleDroneFailed(galaxy::events::DroneFailedEvent event);

private:
    void createFactions(const pg::FrameStamp& frameStamp);

    behavior::ContextPtr ctx{};
};
} // namespace galaxy