#include "StatsSystem.hpp"

#include <pgGame/core/Game.hpp>
#include <components/Faction.hpp>
#include <components/StarSystem.hpp>
#include <iostream>

void galaxy::StatsSystem::setup()
{
    game.addSingleton_as<const Stats&>("galaxy.stats", stats);
    game.getDispatcher().sink<events::DroneFailedEvent>().connect<&StatsSystem::onDroneDestroyed>(this);
    game.getDispatcher().sink<events::DroneCreatedEvent>().connect<&StatsSystem::onDroneCreated>(this);
}

void galaxy::StatsSystem::handle(const pg::game::FrameStamp&)
{
    // count number of starsystems
    auto view = game.getRegistry().view<galaxy::StarSystemState>();
    stats.galaxyStats.numSystems = view.size();
}

void galaxy::StatsSystem::onDroneDestroyed(events::DroneFailedEvent event)
{
    auto  faction = game.getRegistry().get<galaxy::Faction>(event.entity);
    auto& factionStatsValues = stats.factionStats[faction.name];
    factionStatsValues.numDronesFailed++;
}

void galaxy::StatsSystem::onDroneCreated(events::DroneCreatedEvent event)
{
    auto  faction = game.getRegistry().get<galaxy::Faction>(event.entity);
    auto& factionStatsValues = stats.factionStats[faction.name];
    factionStatsValues.numDronesCreated++;
}
