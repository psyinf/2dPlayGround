#include "StatsSystem.hpp"

#include <pgGame/core/Game.hpp>
#include <components/Faction.hpp>
#include <components/StarSystem.hpp>
#include <iostream>

void galaxy::StatsSystem::setup()
{
    game.getCurrentScene().addSingleton_as<const Stats&>("galaxy.stats", stats);
    game.getDispatcher().sink<events::DroneFailedEvent>().connect<&StatsSystem::onDroneDestroyed>(this);
    game.getDispatcher().sink<events::DroneCreatedEvent>().connect<&StatsSystem::onDroneCreated>(this);
}

void galaxy::StatsSystem::handle(const pg::game::FrameStamp&)
{
    auto& fpsCounter = game.getApp().getFPSCounter();
    // count number of starsystems
    auto view = game.getRegistry().view<galaxy::StarSystemState>();
    stats.galaxyStats.numSystems = view.size();
    stats.frameStats.lastFrameFPS = fpsCounter.getLastFrameFPS();
    if (fpsCounter.getCurrentFrameCount() % 20 == 0)
    {
        auto fpsStats = fpsCounter.getAverageFPSAndReset();
        stats.frameStats.fpsStats = fpsStats;
    }
    if (fpsCounter.getCurrentFrameCount() % 100 == 0)
    {
        auto fpsStats = stats.frameStats.fpsStats;
        spdlog::info("FPS Avg/Min/Max: {:.2}/{:.2}/{:.2}", fpsStats.minFPS, fpsStats.maxFPS, fpsStats.averageFPS);
    }
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
