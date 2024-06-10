#pragma once
#include <entt/entt.hpp>
#include <pgGame/systems/SystemInterface.hpp>
#include <pgEngine/math/Vec.hpp>

#include <events/DroneEvents.hpp>
#include <Config.hpp>

#include <unordered_map>

namespace galaxy {
using entt::literals::operator""_hs;

class Game;

class StatsSystem : public pg::game::SystemInterface
{
public:
    struct FactionStatsValues
    {
        uint64_t numDronesFailed;
    };

    using FactionStats = std::unordered_map<std::string, FactionStatsValues>;

public:
    using SystemInterface::SystemInterface;

    void setup() override;

    void handle(const pg::game::FrameStamp& frameStamp) override;

    void onDroneDestroyed(events::DroneFailedEvent event);

private:
    FactionStats factionStats;
};
} // namespace galaxy