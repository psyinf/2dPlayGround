#include "StatsSystem.hpp"

#include <pgGame/core/Game.hpp>
#include <components/Faction.hpp>

void galaxy::StatsSystem::setup()
{
    game.addSingleton_as<const FactionStats&>("galaxy.factionstats", factionStats);
    game.getDispatcher().sink<events::DroneFailedEvent>().connect<&StatsSystem::onDroneDestroyed>(this);
}

void galaxy::StatsSystem::handle(const pg::game::FrameStamp& frameStamp) {}

void galaxy::StatsSystem::onDroneDestroyed(events::DroneFailedEvent event)
{
    auto  faction = game.getRegistry().get<galaxy::Faction>(event.entity);
    auto& factionStatsValues = factionStats[faction.name];
    factionStatsValues.numDronesFailed++;
}
