#pragma once
#include <pgGame/systems/SystemInterface.hpp>
#include <events/DroneEvents.hpp>
#include <components/Stats.hpp>

namespace galaxy {
using entt::literals::operator""_hs;

class Game;

class StatsSystem : public pg::game::SystemInterface
{
public:
public:
    using SystemInterface::SystemInterface;

    void setup(std::string_view scene_id) override;

    void handle(const pg::FrameStamp& frameStamp) override;

    void onDroneDestroyed(events::DroneFailedEvent event);

    void onDroneCreated(events::DroneCreatedEvent event);

private:
    Stats stats;
};
} // namespace galaxy