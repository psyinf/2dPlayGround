#pragma once
#include <entt/entt.hpp>
#include <pgGame/systems/SystemInterface.hpp>
#include <events/DroneEvents.hpp>
#include <behaviors/utils/Context.hpp>

namespace galaxy {
using entt::literals::operator""_hs;

class DroneSystem : public pg::game::SystemInterface
{
public:
    using SystemInterface::SystemInterface;

    void setup(std::string_view scene_id);

    void handle(const pg::FrameStamp& frameStamp);

    void handleDroneFailed(galaxy::events::DroneFailedEvent event);

private:
    void createFactions(const pg::FrameStamp& frameStamp);

    behavior::ContextPtr ctx{};
};
} // namespace galaxy